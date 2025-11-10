#pragma once

#include "main.h"
#include "Component.h"
#include "renderer.h"
#include "vector3.h"
#include "Transform.h"
#include <string>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

// メッシュ形状列挙型
enum class MeshShape
{
	Plane,
	Box,
	Sphere,
	Custom
};

/// <summary>
/// MeshRenderer メッシュの描画を担当するコンポーネント
/// Transformのスケール x ローカルスケール を掛け合わせた値で描画される
/// </summary>
class MeshRenderer : public Component
{	
// --- 変数定義 ---
public:
    // ポインタ類
	ID3D11Buffer*               m_VertexBuffer  = nullptr;
    ID3D11InputLayout*          m_VertexLayout  = nullptr;
    ID3D11VertexShader*         m_VertexShader  = nullptr;
    ID3D11PixelShader*          m_PixelShader   = nullptr;
    ID3D11ShaderResourceView*   m_Texture       = nullptr;
    Transform*                  m_Transform     = nullptr;

    Vector3 m_LocalScale = { 1.0f, 1.0f, 1.0f };            // ローカルスケール
	MeshShape m_Shape = MeshShape::Custom;    			    // メッシュ形状
	XMFLOAT4 m_Color = XMFLOAT4(1, 1, 1, 1);				// メッシュ色
	bool m_EnableTexture = false;                         	// テクスチャ有効フラグ

private:
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // インデックスバッファ
    UINT m_IndexCount = 0;                                	// インデックス数
	UINT m_VertexCount = 0;                             	// 頂点数

// --- 関数定義 ---
public:
	// デフォルトコンストラクタ・デストラクタ
	MeshRenderer() = default;
	~MeshRenderer() override { Release(); }

	/// <summary>
	/// ライフサイクルメソッド
	/// </summary>
	void Init() override {};
	void Uninit() override { Release(); };

    // ----------------------------------------------------------------------
    // 基本操作
    // ----------------------------------------------------------------------
	/// <summary>
	/// テクスチャを設定する
	/// </summary>
	void SetTexture(const std::wstring& filePath)
	{
		TexMetadata Metadata{};
		ScratchImage Image{};
        if (SUCCEEDED(LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &Metadata, Image)))
        {
            CreateShaderResourceView(Renderer::GetDevice(), Image.GetImages(), Image.GetImageCount(), Metadata, &m_Texture);
			m_EnableTexture = true;
        }
	}

	/// <summary>
	/// シェーダーを設定する
	/// </summary>
	void LoadShader(const char* vsFilePath, const char* psFilePath)
	{
        Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, vsFilePath);
        Renderer::CreatePixelShader(&m_PixelShader, psFilePath);
    }

    /// <summary>
    /// ローカルスケールを設定する
    /// </summary>
    void SetLocalScale(float x, float y, float z)
    {
        m_LocalScale = { x, y, z };
    }

    /// ------------------------------------------------------------------------
    /// 単位形状メッシュ作成
    /// ------------------------------------------------------------------------
    /// <summary>
    /// 単位平面メッシュを作成（Plane）
    /// </summary>
    void CreateUnitPlane()
    {
        // 幅1、高さ1の平面を作成
		VERTEX_3D v[4];
		v[0].Position = {-0.5f, 0.0f,  0.5f};
		v[1].Position = { 0.5f, 0.0f,  0.5f};
		v[2].Position = {-0.5f, 0.0f, -0.5f};
		v[3].Position = { 0.5f, 0.0f, -0.5f};

		for (int i = 0; i < 4; ++i)
		{
			v[i].Normal = { 0.0f, 1.0f, 0.0f };
			v[i].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
			v[i].TexCoord = { (i % 2) ? 1.0f : 0.0f, (i / 2) ? 1.0f : 0.0f };
		}
		MakeVertexBuffer(v, 4);
        m_Shape = MeshShape::Plane;
    }

    /// <summary>
    /// 単位立方体メッシュを作成（Box）
    /// </summary>
    void CreateUnitBox()
    {
        const float h = 0.5f;

        // 24頂点（各面4頂点×6面）
        VERTEX_3D v[24] =
        {
            // 前面
            {{-h,-h, h},{0,0,1},{1,1,1,1},{0,1}},
            {{ h,-h, h},{0,0,1},{1,1,1,1},{1,1}},
            {{-h, h, h},{0,0,1},{1,1,1,1},{0,0}},
            {{ h, h, h},{0,0,1},{1,1,1,1},{1,0}},
            // 背面
            {{ h,-h,-h},{0,0,-1},{1,1,1,1},{0,1}},
            {{-h,-h,-h},{0,0,-1},{1,1,1,1},{1,1}},
            {{ h, h,-h},{0,0,-1},{1,1,1,1},{0,0}},
            {{-h, h,-h},{0,0,-1},{1,1,1,1},{1,0}},
            // 右面
            {{ h,-h, h},{1,0,0},{1,1,1,1},{0,1}},
            {{ h, h, h},{1,0,0},{1,1,1,1},{0,0}},
            {{ h,-h,-h},{1,0,0},{1,1,1,1},{1,1}},
            {{ h, h,-h},{1,0,0},{1,1,1,1},{1,0}},
            // 左面
            {{-h,-h,-h},{-1,0,0},{1,1,1,1},{0,1}},
            {{-h, h,-h},{-1,0,0},{1,1,1,1},{0,0}},
            {{-h,-h, h},{-1,0,0},{1,1,1,1},{1,1}},
            {{-h, h, h},{-1,0,0},{1,1,1,1},{1,0}},
            // 上面
            {{-h, h, h},{0,1,0},{1,1,1,1},{0,1}},
            {{-h, h,-h},{0,1,0},{1,1,1,1},{0,0}},
            {{ h, h, h},{0,1,0},{1,1,1,1},{1,1}},
            {{ h, h,-h},{0,1,0},{1,1,1,1},{1,0}},
            // 下面
            {{-h,-h,-h},{0,-1,0},{1,1,1,1},{0,1}},
            {{-h,-h, h},{0,-1,0},{1,1,1,1},{0,0}},
            {{ h,-h,-h},{0,-1,0},{1,1,1,1},{1,1}},
            {{ h,-h, h},{0,-1,0},{1,1,1,1},{1,0}},
        };

        // 36インデックス（各面2三角形×3頂点×6面）
        const uint16_t indices[36] =
            {
                0, 1, 2, 2, 1, 3,       // 前面
                4, 5, 6, 6, 5, 7,       // 背面
                8, 9, 10, 10, 9, 11,    // 右面
                12, 13, 14, 14, 13, 15, // 左面
                16, 17, 18, 18, 17, 19, // 上面
                20, 21, 22, 22, 21, 23  // 下面
            };

        // 頂点バッファとインデックスバッファの作成
        MakeVertexBuffer(v, 24);
        MakeIndexBuffer(indices, 36);
        m_Shape = MeshShape::Box;
    }

    /// <summary>
    /// UnitSphereメッシュを作成
    /// </summary>
    void CreateUnitSphere(int slices = 16, int stacks = 16)
    {
        std::vector<VERTEX_3D> vertices;
        vertices.reserve((stacks + 1) * (slices + 1));

        float radius = 0.5f;

        for (int i = 0; i <= stacks; ++i)
        {
            float v = static_cast<float>(i) / stacks;
            float phi = XM_PI * v;

            for (int j = 0; j <= slices; ++j)
            {
                float u = static_cast<float>(j) / slices;
                float theta = XM_2PI * u;

                Vector3 pos
                {
                    radius * sinf(phi) * cosf(theta),
                    radius * cosf(phi),
                    radius * sinf(phi) * sinf(theta)
                };

            Vector3 norm = pos;
            norm.Normalize();

            VERTEX_3D vertex{};
            vertex.Position = { pos.x, pos.y, pos.z };
            vertex.Normal = { norm.x, norm.y, norm.z };
            vertex.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
            vertex.TexCoord = { u, v };
            vertices.push_back(vertex);
            }
        }

        MakeVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size()));
        m_Shape = MeshShape::Sphere;
    }

    /// ----------------------------------------------------------------------
    /// 描画処理
    /// ----------------------------------------------------------------------
	/// <summary>
	/// メッシュの描画
	/// </summary>
	void Draw() override
    {
        // ローカルスケールを反映したワールド行列を取得
        const auto localScaleMatrix = XMMatrixScaling(m_LocalScale.x, m_LocalScale.y, m_LocalScale.z);
        const auto world = localScaleMatrix * m_Transform->GetWorldMatrix();

        // マテリアル設定
        MATERIAL mat{};
        mat.Diffuse = m_Color;
        mat.Ambient = {1,1,1,1};
        mat.TextureEnable = m_EnableTexture;

        Renderer::SetMaterial(mat);
        Renderer::SetWorldMatrix(world);

        // 描画
        auto ctx = Renderer::GetDeviceContext();
        ctx->IASetInputLayout(m_VertexLayout);
        ctx->VSSetShader(m_VertexShader, nullptr, 0);
        ctx->PSSetShader(m_PixelShader, nullptr, 0);

        UINT stride = sizeof(VERTEX_3D), offset = 0;
        ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
        
        // テクスチャ設定
        if (m_Texture)
        {
            ctx->PSSetShaderResources(0, 1, &m_Texture);
        }

        // インデックスバッファがある場合はそちらを使用
        if (m_IndexBuffer && m_IndexCount > 0)
        {
            ctx->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
            ctx->IASetPrimitiveTopology(GetTopology());
            ctx->DrawIndexed(m_IndexCount, 0, 0);
        }
        else
        {
            ctx->IASetPrimitiveTopology(GetTopology());
            ctx->Draw(m_VertexCount, 0);
        }
    }
	
private:
	/// <summary>
	/// 頂点バッファを作成する
	/// </summary>
    void MakeVertexBuffer(const VERTEX_3D* verts, UINT count)
    {
        m_VertexCount = count;
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(VERTEX_3D) * count;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = verts;

        Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
    }

    /// <summary>
    /// インデックスバッファを作成する
    /// </summary>
    void MakeIndexBuffer(const uint16_t* indices, UINT count)
    {
        m_IndexCount = count;

        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(uint16_t) * count;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = indices;

        Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
    }

	/// <summary>
	/// トポロジーを設定する
	/// </summary>
	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const
	{
		switch (m_Shape)
		{
			case MeshShape::Plane:
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			case MeshShape::Box:
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case MeshShape::Sphere:
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			default:
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}

	// リソース解放
    void Release()
    {
        if (m_Texture)       { m_Texture->Release(); m_Texture = nullptr; }
        if (m_VertexBuffer)  { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
        if (m_VertexLayout)  { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
        if (m_VertexShader)  { m_VertexShader->Release(); m_VertexShader = nullptr; }
        if (m_PixelShader)   { m_PixelShader->Release(); m_PixelShader = nullptr; }
        if (m_IndexBuffer)   { m_IndexBuffer->Release(); m_IndexBuffer = nullptr; }
    }
};