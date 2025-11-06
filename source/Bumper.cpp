#include "main.h"
#include "renderer.h"
#include "Bumper.h"
#include "modelRenderer.h"
#include <math.h>

// XZ平面の内積
static inline float DotXZ(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.z * b.z;
}

// --- GameObject ---
void Bumper::Init()
{
    // モデル読み込み
    m_ModelRenderer = new ModelRenderer();
    m_ModelRenderer->Load("asset\\model\\BumperTest.obj");

    // シェーダー作成
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

// 終了処理
void Bumper::Uninit()
{
    // モデルレンダラーの解放
    delete m_ModelRenderer;
    m_ModelRenderer = nullptr;

    // シェーダーの解放
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();
}

// 更新処理
void Bumper::Update() {}

// 描画処理
void Bumper::Draw()
{
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);

    // ワールド行列（XY回転はなし。XZスケールで半径に合わせ、高さは0.4）
    const float h = 0.4f;
    XMMATRIX scale = XMMatrixScaling(m_Radius, h, m_Radius);
    // 床が y=0 なので、薄い円柱の中心を y=h/2 に上げて見切れないようにする
    XMMATRIX trans = XMMatrixTranslation(m_Center.x, h * 0.5f, m_Center.z);
    XMMATRIX world = scale * trans;
    Renderer::SetWorldMatrix(world);

    m_ModelRenderer->Draw();
}

// --- Ball ---
void Bumper::Resolve(Vector3& ballPosition, Vector3& ballVelocity, float ballRadius)
{
    // ボールのXZ平面での衝突判定
    Vector3 n = { ballPosition.x - m_Center.x, 0.0f, ballPosition.z - m_Center.z };
    float d2 = DotXZ(n, n);
    const float minD = ballRadius + m_Radius;

    if (d2 > 0.0f && d2 < minD * minD)
    {
        float d = sqrtf(d2);
        if (d < 1e-6f) {
            // 法線ベクトルの設定
            n = { 1.0f, 0.0f, 0.0f };
            d = 1.0f;
        }
        Vector3 nh = { n.x / d, 0.0f, n.z / d };

        // 1) 位置補正
        ballPosition += nh * (minD - d);

        // 2) 速度補正
        float vn = DotXZ(ballVelocity, nh);         // 法線方向の速度成分
        // 反発係数を考慮した速度補正
        ballVelocity = ballVelocity - nh * (1.0f + m_Restitution) * vn + nh * m_KickVelocity;
    }
}
