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

// ���b�V���`��񋓌^
enum class MeshShape
{
	Plane,
	Box,
	Sphere,
	Custom
};

/// <summary>
/// MeshRenderer ���b�V���̕`���S������R���|�[�l���g
/// Transform�̃X�P�[�� x ���[�J���X�P�[�� ��|�����킹���l�ŕ`�悳���
/// TODO: �V�F�[�_�[���ݒ肳��Ă��Ȃ��ꍇ�̃f�t�H���g�V�F�[�_�[������ǉ�
/// </summary>
class MeshRenderer : public Component
{	
// --- �ϐ���` ---
public:
    // �|�C���^��
	ID3D11Buffer*               m_VertexBuffer  = nullptr;
    ID3D11InputLayout*          m_VertexLayout  = nullptr;
    ID3D11VertexShader*         m_VertexShader  = nullptr;
    ID3D11PixelShader*          m_PixelShader   = nullptr;
    ID3D11ShaderResourceView*   m_Texture       = nullptr;
    Transform*                  m_Transform     = nullptr;

    Vector3 m_LocalScale = { 1.0f, 1.0f, 1.0f };            // ���[�J���X�P�[��
	MeshShape m_Shape = MeshShape::Custom;    			    // ���b�V���`��
	XMFLOAT4 m_Color = XMFLOAT4(1, 1, 1, 1);				// ���b�V���F�i�f�t�H���g�͔��j
	bool m_EnableTexture = false;                         	// �e�N�X�`���L���t���O

private:
    ID3D11Buffer* m_IndexBuffer = nullptr;                  // �C���f�b�N�X�o�b�t�@
    UINT m_IndexCount = 0;                                	// �C���f�b�N�X��
	UINT m_VertexCount = 0;                             	// ���_��

// --- �֐���` ---
public:
	// �f�t�H���g�R���X�g���N�^�E�f�X�g���N�^
	MeshRenderer() = default;
	~MeshRenderer() override { Release(); }

	/// <summary>
	/// ���C�t�T�C�N�����\�b�h
	/// </summary>
	void Init() override {};
	void Uninit() override { Release(); };

    // ----------------------------------------------------------------------
    // ��{����
    // ----------------------------------------------------------------------
	/// <summary>
	/// �e�N�X�`����ݒ肷��
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
	/// �V�F�[�_�[��ݒ肷��
	/// </summary>
	void LoadShader(const char* vsFilePath, const char* psFilePath)
	{
        Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, vsFilePath);
        Renderer::CreatePixelShader(&m_PixelShader, psFilePath);
    }

    /// <summary>
    /// ���[�J���X�P�[����ݒ肷��
    /// </summary>
    void SetLocalScale(float x, float y, float z)
    {
        m_LocalScale = { x, y, z };
    }

    /// ------------------------------------------------------------------------
    /// �P�ʌ`�󃁃b�V���쐬
    /// ------------------------------------------------------------------------
    /// <summary>
    /// �P�ʕ��ʃ��b�V����쐬�iPlane�j
    /// </summary>
    void CreateUnitPlane()
    {
        // ��1�A����1�̕��ʂ�쐬
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
    /// �P�ʗ����̃��b�V����쐬�iBox�j
    /// </summary>
    void CreateUnitBox()
    {
        const float h = 0.5f;

        // 24���_�i�e��4���_�~6�ʁj
            // 24���_�i�e��4���_�~6�ʁj
        VERTEX_3D v[24] =
        {
            // �O�� (+Z)
            {{-h,-h, h},{0,0,1},{1,1,1,1},{0,1}},
            {{ h,-h, h},{0,0,1},{1,1,1,1},{1,1}},
            {{-h, h, h},{0,0,1},{1,1,1,1},{0,0}},
            {{ h, h, h},{0,0,1},{1,1,1,1},{1,0}},
            // �w�� (-Z)
            {{ h,-h,-h},{0,0,-1},{1,1,1,1},{0,1}},
            {{-h,-h,-h},{0,0,-1},{1,1,1,1},{1,1}},
            {{ h, h,-h},{0,0,-1},{1,1,1,1},{0,0}},
            {{-h, h,-h},{0,0,-1},{1,1,1,1},{1,0}},
            // �E�� (+X)
            {{ h,-h, h},{1,0,0},{1,1,1,1},{0,1}},
            {{ h, h, h},{1,0,0},{1,1,1,1},{0,0}},
            {{ h,-h,-h},{1,0,0},{1,1,1,1},{1,1}},
            {{ h, h,-h},{1,0,0},{1,1,1,1},{1,0}},
            // ���� (-X)
            {{-h,-h,-h},{-1,0,0},{1,1,1,1},{0,1}},
            {{-h, h,-h},{-1,0,0},{1,1,1,1},{0,0}},
            {{-h,-h, h},{-1,0,0},{1,1,1,1},{1,1}},
            {{-h, h, h},{-1,0,0},{1,1,1,1},{1,0}},
            // ��� (+Y)
            {{-h, h, h},{0,1,0},{1,1,1,1},{0,1}},
            {{-h, h,-h},{0,1,0},{1,1,1,1},{0,0}},
            {{ h, h, h},{0,1,0},{1,1,1,1},{1,1}},
            {{ h, h,-h},{0,1,0},{1,1,1,1},{1,0}},
            // ���� (-Y)
            {{-h,-h,-h},{0,-1,0},{1,1,1,1},{0,1}},
            {{-h,-h, h},{0,-1,0},{1,1,1,1},{0,0}},
            {{ h,-h,-h},{0,-1,0},{1,1,1,1},{1,1}},
            {{ h,-h, h},{0,-1,0},{1,1,1,1},{1,0}},
        };

        // 36�C���f�b�N�X�i�e��2�O�p�`�~3���_�~6�ʁj
        const uint16_t indices[36] =
        {
         0,  1,  2,  2,  1,  3,  // �O��
         4,  5,  6,  6,  5,  7,  // �w��
         8, 10,  9,  9, 10, 11,
        14, 15, 13, 14, 13, 12,
        16, 18, 17, 18, 19, 17,
        20, 22, 21, 21, 22, 23,
    };

        // ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̍쐬
        MakeVertexBuffer(v, 24);
        MakeIndexBuffer(indices, 36);
        m_Shape = MeshShape::Box;
    }

    /// <summary>
    /// UnitSphere���b�V����쐬
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
    /// �`�揈��
    /// ----------------------------------------------------------------------
	/// <summary>
	/// ���b�V���̕`��
	/// </summary>
	void Draw() override
    {
        // ���[�J���X�P�[���𔽉f�������[���h�s���擾
        const auto localScaleMatrix = XMMatrixScaling(m_LocalScale.x, m_LocalScale.y, m_LocalScale.z);
        const auto world = localScaleMatrix * m_Transform->GetWorldMatrix();

        // �}�e���A���ݒ�
        MATERIAL mat{};
        mat.Diffuse = m_Color;
        mat.Ambient = {1,1,1,1};
        mat.TextureEnable = m_EnableTexture;

        Renderer::SetMaterial(mat);
        Renderer::SetWorldMatrix(world);

        // �`��
        auto ctx = Renderer::GetDeviceContext();
        ctx->IASetInputLayout(m_VertexLayout);
        ctx->VSSetShader(m_VertexShader, nullptr, 0);
        ctx->PSSetShader(m_PixelShader, nullptr, 0);

        UINT stride = sizeof(VERTEX_3D), offset = 0;
        ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
        
        // �e�N�X�`���ݒ�
        if (m_Texture)
        {
            ctx->PSSetShaderResources(0, 1, &m_Texture);
        }

        // �C���f�b�N�X�o�b�t�@������ꍇ�͂������g�p
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
	/// ���_�o�b�t�@��쐬����
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
    /// �C���f�b�N�X�o�b�t�@��쐬����
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
	/// �g�|���W�[��ݒ肷��
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

	// ���\�[�X���
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