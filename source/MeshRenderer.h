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

	MeshShape m_Shape = MeshShape::Box;    					// メッシュ形状
	XMFLOAT4 m_Color = XMFLOAT4(1, 1, 1, 1);				// メッシュ色
	bool m_EnableTexture = false;                         	// テクスチャ有効フラグ

private:
	UINT m_VertexCount = 0;                             	// 頂点数

// --- 関数定義 ---
public:
	// デフォルトコンストラクタ・デストラクタ
	MeshRenderer() = default;
	~MeshRenderer() override { Release(); }

	/// <summary>
	/// ライフサイクルメソッド
	/// </summary>
	void Init() override;
	void Uninit() override;

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
	/// メッシュ形状を設定する
	/// </summary>
	void SetMeshShape(MeshShape shape) { m_Shape = shape; }

	/// <summary>
	/// Planeメッシュを作成
	/// </summary>
	void CreatePlane(float width, float height)
	{
		float w = width / 2.0f, h = height / 2.0f;
		VERTEX_3D v[4];

		v[0].Position = {-w, 0.0f, -h};
		v[1].Position = { w, 0.0f, -h};
		v[2].Position = {-w, 0.0f,  h};
		v[3].Position = { w, 0.0f,  h};

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
	/// Boxメッシュを作成
	/// </summary>
	void CreateBox(float w, float h, float d)
    {
        const float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
        VERTEX_3D v[] =
        {
            // 前面
            {{-hw,-hh,-hd}, {0,0,-1}, {1,1,1,1}, {0,1}},
            {{-hw, hh,-hd}, {0,0,-1}, {1,1,1,1}, {0,0}},
            {{ hw,-hh,-hd}, {0,0,-1}, {1,1,1,1}, {1,1}},
            {{ hw, hh,-hd}, {0,0,-1}, {1,1,1,1}, {1,0}},
            // 背面
            {{-hw,-hh, hd}, {0,0,1}, {1,1,1,1}, {1,1}},
            {{ hw,-hh, hd}, {0,0,1}, {1,1,1,1}, {0,1}},
            {{-hw, hh, hd}, {0,0,1}, {1,1,1,1}, {1,0}},
            {{ hw, hh, hd}, {0,0,1}, {1,1,1,1}, {0,0}},
        };

        // 今回は四角形×2 = 6面にしてもOKだけど、簡易的に描画する場合は前後だけでもよい。
        MakeVertexBuffer(v, _countof(v));
        m_Shape = MeshShape::Box;
    }

	/// <summary>
	/// 球メッシュを作成
	/// </summary>
	void CreateSphere(float radius, int slices = 16, int stacks = 16)
    {
        std::vector<VERTEX_3D> vertices;
        for (int i = 0; i <= stacks; i++)
        {
            float v = (float)i / stacks;
            float phi = XM_PI * v;
            for (int j = 0; j <= slices; j++)
            {
                float u = (float)j / slices;
                float theta = XM_2PI * u;

                Vector3 pos{
                    radius * sinf(phi) * cosf(theta),
                    radius * cosf(phi),
                    radius * sinf(phi) * sinf(theta)
                };
                Vector3 n = pos;
                n.Normalize();

                VERTEX_3D vertex{};
                vertex.Position = { pos.x, pos.y, pos.z };
                vertex.Normal = { n.x, n.y, n.z };
                vertex.Diffuse = {1,1,1,1};
                vertex.TexCoord = {u, v};
        
                vertices.push_back(vertex);
            }
        }
        MakeVertexBuffer(vertices.data(), (UINT)vertices.size());
        m_Shape = MeshShape::Sphere;
    }

	/// <summary>
	/// メッシュの描画
	/// </summary>
	void Draw() override
    {
        // ワールド行列の取得
        XMMATRIX world = m_Transform->GetWorldMatrix();

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

        if (m_Texture)
            ctx->PSSetShaderResources(0, 1, &m_Texture);

        ctx->IASetPrimitiveTopology(GetTopology());
        ctx->Draw(m_VertexCount, 0);
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
    }
};