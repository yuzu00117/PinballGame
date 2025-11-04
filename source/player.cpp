#include "main.h"
#include "renderer.h"
#include "player.h"
#include "modelRenderer.h"
#include "camera.h"
#include "manager.h"
#include <Windows.h>

void Player::Init()
{
	m_Position = { 0.0f, 1.0f, 0.0f };
	m_Rotation = { 0.0f, 0.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };

	m_ModelRenderer = new ModelRenderer();
	m_ModelRenderer->Load("asset\\model\\player.obj");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Player::Uninit()
{
	delete m_ModelRenderer;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Player::Update()
{
    float speed = 0.1f; // 移動速度
    
    // カメラの取得
    Camera* camera = nullptr;
    for (auto obj : Manager::GetGameObjects()) {
        camera = dynamic_cast<Camera*>(obj);
        if (camera) break;
    }
    
    if (camera) {
        // カメラのYaw角度を使ってプレイヤーの向きを決定
        m_Rotation.y = camera->GetYaw();
        
        // カメラの向きに合わせた移動方向を計算
        float forward = 0.0f;  // 前後の移動量
        float strafe = 0.0f;   // 左右の移動量
        
        // Wキー（前進）
        if (GetAsyncKeyState('W') & 0x8000) {
            forward -= speed;
			// 前を向く
			m_Rotation.y = camera->GetYaw(); // カメラのYawを使用して前を向く
        }
        // Sキー（後退）
        if (GetAsyncKeyState('S') & 0x8000) {
            forward += speed;
            // 後ろを向く
			m_Rotation.y += XM_PI; // 180度回転
        }
        // Aキー（左）
        if (GetAsyncKeyState('A') & 0x8000) {
            strafe += speed;
			// 左を向く
			m_Rotation.y -= XM_PIDIV2; // 90度左回転
        }
        // Dキー（右）
        if (GetAsyncKeyState('D') & 0x8000) {
            strafe -= speed;
			// 右を向く
			m_Rotation.y += XM_PIDIV2; // 90度右回転
        }
        
        // カメラの角度に基づいて移動方向を変換
        float yaw = camera->GetYaw();
        
        // 前後方向の移動
        m_Position.x += sinf(yaw) * forward;
        m_Position.z += cosf(yaw) * forward;
        
        // 左右方向の移動（横方向はYaw + 90度）
        m_Position.x += sinf(yaw + XM_PIDIV2) * strafe;
        m_Position.z += cosf(yaw + XM_PIDIV2) * strafe;
    }
    else {
        // カメラが見つからない場合は従来の移動方法を使用
        if (GetAsyncKeyState('W') & 0x8000) {
            m_Position.z -= speed;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            m_Position.z += speed;
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            m_Position.x -= speed;
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            m_Position.x += speed;
        }
    }
}

void Player::Draw()
{
    // マテリアル設定は、mtlファイルを使うのでなし

    //入力レイアウト
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    //シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // テクスチャ設定
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    //マトリクス設定
    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
    rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);
    m_ModelRenderer->Draw();
}