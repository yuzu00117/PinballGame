#include "main.h"
#include "renderer.h"
#include "field.h"
#include "MathUtil.h"
#include <algorithm>
#include <cmath>

// --- XZ 2D ヘルパ ---
static inline Vector3 XZ(const Vector3& v){ return {v.x, 0.0f, v.z}; }
static inline float   dotXZ(const Vector3& a, const Vector3& b){ return a.x*b.x + a.z*b.z; }
static inline float   lenXZ(const Vector3& v){ return std::sqrt(v.x*v.x + v.z*v.z); }
static float closestT(const Vector3& p, const Vector3& a, const Vector3& b){
    Vector3 ap = XZ(p - a), ab = XZ(b - a);
    float ab2 = dotXZ(ab, ab);
    if (ab2 <= 1e-6f) return 0.0f;
    float t = dotXZ(ap, ab) / ab2;
    return Clamp(t, 0.0f, 1.0f);
}

// ----------------- GameObject 基本 -----------------
void Field::Init()
{
    // --- 床メッシュ（既存どおり） ---
    VERTEX_3D v[4];
    v[0].Position = XMFLOAT3(-9.5f, 0.0f,  14.5f); v[0].Normal = {0,1,0}; v[0].Diffuse = {1,1,1,1}; v[0].TexCoord = {0,0};
    v[1].Position = XMFLOAT3( 9.5f, 0.0f,  14.5f); v[1].Normal = {0,1,0}; v[1].Diffuse = {1,1,1,1}; v[1].TexCoord = {1,0};
    v[2].Position = XMFLOAT3(-9.5f, 0.0f, -14.5f); v[2].Normal = {0,1,0}; v[2].Diffuse = {1,1,1,1}; v[2].TexCoord = {0,1};
    v[3].Position = XMFLOAT3( 9.5f, 0.0f, -14.5f); v[3].Normal = {0,1,0}; v[3].Diffuse = {1,1,1,1}; v[3].TexCoord = {1,1};

    D3D11_BUFFER_DESC bd{}; bd.Usage = D3D11_USAGE_DEFAULT; bd.ByteWidth = sizeof(VERTEX_3D) * 4; bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA sd{}; sd.pSysMem = v;
    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    TexMetadata metadata; ScratchImage image;
    LoadFromWICFile(L"asset\\texture\\field004.jpg", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);

    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

    // --- 追加：外周の壁を構築（枠だけ） ---
    BuildDefaultFrame(); // X:[-9.5,9.5], Z:[-14.5,14.5], 厚み0.2
}

void Field::Uninit()
{
    if (m_Texture)      { m_Texture->Release();      m_Texture = nullptr; }
    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
    if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
    if (m_PixelShader)  { m_PixelShader->Release();  m_PixelShader = nullptr; }
}

void Field::Update()
{
    // 今は特に無し（将来：テクスチャスクロール等）
}

void Field::Draw()
{
    // マテリアル
    MATERIAL mat{}; mat.Ambient = {1,1,1,1}; mat.Diffuse = {1,1,1,1}; mat.Specular = {0,0,0,0}; mat.Shininess = 1.0f; mat.TextureEnable = true;
    Renderer::SetMaterial(mat);

    // 入力レイアウト/シェーダ
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // ワールド
    XMMATRIX world = XMMatrixIdentity();
    Renderer::SetWorldMatrix(world);

    // 頂点バッファ
    UINT stride = sizeof(VERTEX_3D), offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Renderer::GetDeviceContext()->Draw(4, 0);

    // ※ 壁の描画は後で（今は物理だけ用意）
}

// ----------------- 壁管理 -----------------
void Field::ClearWalls(){ m_Walls.clear(); }

void Field::AddWall(const WALL_SEGMENT& w){ m_Walls.push_back(w); }

void Field::BuildDefaultFrame(float halfW, float halfH, float thick)
{
    m_Walls.clear();
    // 外周4辺（角はカプセル太さで管理）
    m_Walls.push_back({{-halfW,0,-halfH},{ halfW,0,-halfH}, thick, 0.8f}); // 下
    m_Walls.push_back({{ halfW,0,-halfH},{ halfW,0, halfH}, thick, 0.8f}); // 右
    m_Walls.push_back({{ halfW,0, halfH},{-halfW,0, halfH}, thick, 0.8f}); // 上
    m_Walls.push_back({{-halfW,0, halfH},{-halfW,0,-halfH}, thick, 0.8f}); // 左
}

// ----------------- 衝突解決（Ball から呼ぶ） -----------------
void Field::ResolveBallCollision(Vector3& pos, Vector3& vel, float ballR) const
{
    for (const auto& w : m_Walls){
        float t = closestT(pos, w.Start, w.End);
        Vector3 q = w.Start + (w.End - w.Start) * t;   // 線分最近点
        Vector3 n = XZ(pos - q);
        float d2 = dotXZ(n, n);
        float minDist = ballR + w.Radius;

        if (d2 > 0.0f && d2 < minDist*minDist){
            float d = std::sqrt(d2);
            Vector3 nhat = { n.x/d, 0.0f, n.z/d };

            // めり込み解消
            pos += nhat * (minDist - d);

            // 法線反射（近づくときのみ）
            float vn = dotXZ(vel, nhat);
            if (vn < 0.0f){
                Vector3 vt = vel - nhat * vn;             // 接線
                vel = vt - nhat * vn * w.Restitution;     // 反発
                // ほんの少し減衰
                vel.x *= 0.995f; vel.z *= 0.995f;
            }
        }
    }
}
