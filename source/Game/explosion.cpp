//#include "main.h"
//#include "renderer.h"
//#include "explosion.h"
//#include "camera.h"
//#include "GameManager.h"
//
//void Explosion::Init()
//{
//    VERTEX_3D vertex[4];
//
//    // XY���ʁiZ=0�j�Ŕ�쐬
//    vertex[0].Position = XMFLOAT3(-10.0f, 10.0f, 0.0f);
//    vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
//    vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//    vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
//
//    vertex[1].Position = XMFLOAT3(10.0f, 10.0f, 0.0f);
//    vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
//    vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//    vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
//
//    vertex[2].Position = XMFLOAT3(-10.0f, -10.0f, 0.0f);
//    vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
//    vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//    vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
//
//    vertex[3].Position = XMFLOAT3(10.0f, -10.0f, 0.0f);
//    vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
//    vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//    vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
//
//    // ���_�o�b�t�@����
//    D3D11_BUFFER_DESC bd;
//    ZeroMemory(&bd, sizeof(bd));
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
//    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    bd.CPUAccessFlags = 0;
//
//    D3D11_SUBRESOURCE_DATA sd;
//    sd.pSysMem = vertex;
//
//    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
//
//    // �e�N�X�`���ǂݍ���
//    TexMetadata metadata;
//    ScratchImage image;
//    LoadFromWICFile(L"asset\\texture\\explosion.png", WIC_FLAGS_NONE, &metadata, image);
//    CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
//    assert(m_Texture);
//
//    // �V�F�[�_�[�ǂݍ���
//    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\bin\\unlitTextureVS.cso");
//    Renderer::CreatePixelShader(&m_PixelShader, "shader\\bin\\unlitTexturePS.cso");
//}
//
//void Explosion::Uninit()
//{
//    m_Texture->Release();
//    m_VertexBuffer->Release();
//    m_VertexLayout->Release();
//    m_VertexShader->Release();
//    m_PixelShader->Release();
//}
//
//void Explosion::Update()
//{
//    // �o�ߎ��Ԃ���Z
//    m_ElapsedTime += 1.0f / 60.0f; // 60FPS�O��B���ۂ�dt��n���̂����z
//
//    if (m_ElapsedTime >= m_FrameTime) {
//        m_ElapsedTime = 0.0f;
//        m_CurrentFrame++;
//        if (m_CurrentFrame >= m_TotalFrames) {
//            m_CurrentFrame = 0; // ���[�v�B���������ꍇ��Uninit��
//        }
//    }
//}
//
//void Explosion::Draw()
//{
//    // �A���t�@�u�����h�L����
//    Renderer::SetATCEnable(true); // �ʏ�̃A���t�@�u�����h
//
//    // ���̓��C�A�E�g�ݒ�
//    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
//
//    // �V�F�[�_�[�ݒ�
//    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
//    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
//
//    // �X�v���C�g�V�[�g��UV�v�Z
//    int frameX = m_CurrentFrame % m_FramesPerRow;
//    int frameY = m_CurrentFrame / m_FramesPerRow;
//    float u0 = 1.0f / m_FramesPerRow * frameX;
//    float v0 = 1.0f / m_FramesPerRow * frameY;
//    float u1 = u0 + 1.0f / m_FramesPerRow;
//    float v1 = v0 + 1.0f / m_FramesPerRow;
//
//    // ���_�f�[�^��s�x�����i�{���̓o�b�t�@��g���񂷂̂����z�j
//    VERTEX_3D vertex[4];
//    float size = 5.0f;
//    vertex[0].Position = XMFLOAT3(-size, size, 0.0f); vertex[0].TexCoord = XMFLOAT2(u0, v0);
//    vertex[1].Position = XMFLOAT3(size, size, 0.0f); vertex[1].TexCoord = XMFLOAT2(u1, v0);
//    vertex[2].Position = XMFLOAT3(-size, -size, 0.0f); vertex[2].TexCoord = XMFLOAT2(u0, v1);
//    vertex[3].Position = XMFLOAT3(size, -size, 0.0f); vertex[3].TexCoord = XMFLOAT2(u1, v1);
//
//    // ���_�o�b�t�@��X�V
//    Renderer::GetDeviceContext()->UpdateSubresource(m_VertexBuffer, 0, NULL, vertex, 0, 0);
//
//    // �J�����̃r���[�s��擾
//    Camera* camera = nullptr;
//    for (auto obj : GameManager::GetGameObjects()) {
//        camera = dynamic_cast<Camera*>(obj);
//        if (camera) break;
//    }
//
//    XMMATRIX world, scale, rot, trans;
//    scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
//
//    if (camera) {
//        // �Q�b�^�[�Ńr���[�s���擾���A�r���{�[�h�s���쐬
//        XMMATRIX view = camera->GetViewMatrix();
//        XMMATRIX invView = XMMatrixInverse(nullptr, view);
//        // ��]�����̂ݒ��o
//        invView.r[3] = XMVectorSet(0, 0, 0, 1);
//        rot = invView;
//    }
//    else {
//        rot = XMMatrixIdentity();
//    }
//
//    world = scale * rot * trans;
//    Renderer::SetWorldMatrix(world);
//
//    // ���_�o�b�t�@�ݒ�
//    UINT stride = sizeof(VERTEX_3D);
//    UINT offset = 0;
//    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
//
//    // �e�N�X�`���ݒ�
//    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
//
//    // �v���~�e�B�u�g�|���W�ݒ�
//    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//    // �|���S���`��
//    Renderer::GetDeviceContext()->Draw(4, 0);
//}