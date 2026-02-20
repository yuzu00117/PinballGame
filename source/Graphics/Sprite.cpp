#include "main.h"
#include "renderer.h"
#include "Sprite.h"

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - 動的頂点バッファを生成する（毎フレーム Map/Unmap で書き換えるため DYNAMIC）
// - BaseLit シェーダーを読み込む
// NOTE: テクスチャは SetTexture() で別途設定すること
void Sprite::Init()
{
    D3D11_BUFFER_DESC bd{};
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth      = sizeof(VERTEX_3D) * 4;
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    Renderer::GetDevice()->CreateBuffer(&bd, nullptr, &m_VertexBuffer);

    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\bin\\BaseLitVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\bin\\BaseLitPS.cso");
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - 所有するすべての D3D リソースを解放する
void Sprite::Uninit()
{
    if (m_Texture)      { m_Texture->Release();      m_Texture      = nullptr; }
    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
    if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
    if (m_PixelShader)  { m_PixelShader->Release();  m_PixelShader  = nullptr; }
}

// ------------------------------------------------------------------------------
// テクスチャ設定
// ------------------------------------------------------------------------------
// - 既存テクスチャがある場合は先に解放してから再読み込みする
// - WIC 対応形式（png / jpg / bmp）を指定すること
void Sprite::SetTexture(const std::wstring& filePath)
{
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = nullptr;
    }

    TexMetadata  metadata{};
    ScratchImage image{};
    if (SUCCEEDED(LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &metadata, image)))
    {
        CreateShaderResourceView(
            Renderer::GetDevice(),
            image.GetImages(), image.GetImageCount(), metadata,
            &m_Texture);
    }
}

// ------------------------------------------------------------------------------
// 頂点バッファ更新（private）
// ------------------------------------------------------------------------------
// - メンバパラメータから 4 頂点を構築し、動的頂点バッファへ書き込む
// - 頂点配置（TRIANGLE_STRIP 順）:
//     v0(左上) --- v1(右上)
//       |               |
//     v2(左下) --- v3(右下)
// NOTE: Draw() から毎フレーム呼ばれる
void Sprite::UpdateVertexBuffer()
{
    if (!m_VertexBuffer) return;

    const float left   = m_X;
    const float top    = m_Y;
    const float right  = m_X + m_Width;
    const float bottom = m_Y + m_Height;

    const float u0 = m_UV[0];
    const float v0 = m_UV[1];
    const float u1 = m_UV[2];
    const float v1 = m_UV[3];

    VERTEX_3D verts[4]{};
    verts[0].Position = XMFLOAT3(left,  top,    0.0f); verts[0].TexCoord = XMFLOAT2(u0, v0); verts[0].Diffuse = m_Color;
    verts[1].Position = XMFLOAT3(right, top,    0.0f); verts[1].TexCoord = XMFLOAT2(u1, v0); verts[1].Diffuse = m_Color;
    verts[2].Position = XMFLOAT3(left,  bottom, 0.0f); verts[2].TexCoord = XMFLOAT2(u0, v1); verts[2].Diffuse = m_Color;
    verts[3].Position = XMFLOAT3(right, bottom, 0.0f); verts[3].TexCoord = XMFLOAT2(u1, v1); verts[3].Diffuse = m_Color;

    auto* ctx = Renderer::GetDeviceContext();
    D3D11_MAPPED_SUBRESOURCE msr{};
    if (SUCCEEDED(ctx->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
    {
        memcpy(msr.pData, verts, sizeof(verts));
        ctx->Unmap(m_VertexBuffer, 0);
    }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 頂点バッファを最新パラメータで更新してからスクリーン座標で描画する
// - 深度テストを無効化して描画し、終了後に復元する
// NOTE: SetWorldViewProjection2D() はワールド行列を Identity にリセットする
void Sprite::Draw()
{
    if (!m_VertexBuffer) return;

    UpdateVertexBuffer();

    auto* ctx = Renderer::GetDeviceContext();

    Renderer::SetWorldViewProjection2D();

    MATERIAL mat{};
    mat.Diffuse       = m_Color;
    mat.Ambient       = { 1.0f, 1.0f, 1.0f, 1.0f };
    mat.TextureEnable = (m_Texture != nullptr) ? TRUE : FALSE;
    Renderer::SetMaterial(mat);

    Renderer::SetDepthEnable(false);

    ctx->IASetInputLayout(m_VertexLayout);
    ctx->VSSetShader(m_VertexShader, nullptr, 0);
    ctx->PSSetShader(m_PixelShader,  nullptr, 0);

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    if (m_Texture)
        ctx->PSSetShaderResources(0, 1, &m_Texture);

    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    ctx->Draw(4, 0);

    Renderer::SetDepthEnable(true);
}
