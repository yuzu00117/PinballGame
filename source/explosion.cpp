#include "main.h"
#include "renderer.h"
#include "explosion.h"
#include "camera.h"
#include "manager.h"

void Explosion::Init()
{
    VERTEX_3D vertex[4];

    // XY平面（Z=0）で板を作成
    vertex[0].Position = XMFLOAT3(-10.0f, 10.0f, 0.0f);
    vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
    vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

    vertex[1].Position = XMFLOAT3(10.0f, 10.0f, 0.0f);
    vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
    vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

    vertex[2].Position = XMFLOAT3(-10.0f, -10.0f, 0.0f);
    vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
    vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

    vertex[3].Position = XMFLOAT3(10.0f, -10.0f, 0.0f);
    vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
    vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = vertex;

    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    // テクスチャ読み込み
    TexMetadata metadata;
    ScratchImage image;
    LoadFromWICFile(L"asset\\texture\\explosion.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
    assert(m_Texture);

    // シェーダー読み込み
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Explosion::Uninit()
{
    m_Texture->Release();
    m_VertexBuffer->Release();
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();
}

void Explosion::Update()
{
    // 経過時間を加算
    m_ElapsedTime += 1.0f / 60.0f; // 60FPS前提。実際はdtを渡すのが理想

    if (m_ElapsedTime >= m_FrameTime) {
        m_ElapsedTime = 0.0f;
        m_CurrentFrame++;
        if (m_CurrentFrame >= m_TotalFrames) {
            m_CurrentFrame = 0; // ループ。消したい場合はUninit等
        }
    }
}

void Explosion::Draw()
{
    // アルファブレンド有効化
    Renderer::SetATCEnable(true); // 通常のアルファブレンド

    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // スプライトシートのUV計算
    int frameX = m_CurrentFrame % m_FramesPerRow;
    int frameY = m_CurrentFrame / m_FramesPerRow;
    float u0 = 1.0f / m_FramesPerRow * frameX;
    float v0 = 1.0f / m_FramesPerRow * frameY;
    float u1 = u0 + 1.0f / m_FramesPerRow;
    float v1 = v0 + 1.0f / m_FramesPerRow;

    // 頂点データを都度生成（本来はバッファを使い回すのが理想）
    VERTEX_3D vertex[4];
    float size = 5.0f;
    vertex[0].Position = XMFLOAT3(-size, size, 0.0f); vertex[0].TexCoord = XMFLOAT2(u0, v0);
    vertex[1].Position = XMFLOAT3(size, size, 0.0f); vertex[1].TexCoord = XMFLOAT2(u1, v0);
    vertex[2].Position = XMFLOAT3(-size, -size, 0.0f); vertex[2].TexCoord = XMFLOAT2(u0, v1);
    vertex[3].Position = XMFLOAT3(size, -size, 0.0f); vertex[3].TexCoord = XMFLOAT2(u1, v1);

    // 頂点バッファを更新
    Renderer::GetDeviceContext()->UpdateSubresource(m_VertexBuffer, 0, NULL, vertex, 0, 0);

    // カメラのビュー行列取得
    Camera* camera = nullptr;
    for (auto obj : Manager::GetGameObjects()) {
        camera = dynamic_cast<Camera*>(obj);
        if (camera) break;
    }

    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

    if (camera) {
        // ゲッターでビュー行列を取得し、ビルボード行列を作成
        XMMATRIX view = camera->GetViewMatrix();
        XMMATRIX invView = XMMatrixInverse(nullptr, view);
        // 回転成分のみ抽出
        invView.r[3] = XMVectorSet(0, 0, 0, 1);
        rot = invView;
    }
    else {
        rot = XMMatrixIdentity();
    }

    world = scale * rot * trans;
    Renderer::SetWorldMatrix(world);

    // 頂点バッファ設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // テクスチャ設定
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // プリミティブトポロジ設定
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ポリゴン描画
    Renderer::GetDeviceContext()->Draw(4, 0);
}