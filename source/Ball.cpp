#include "main.h"
#include "renderer.h"
#include "ball.h"
#include "modelRenderer.h"
#include "AnimationModel.h"
#include "camera.h"
#include "manager.h"
#include <Windows.h>

// デフォルトのボールスケール定義
const Vector3 Ball::DefaultBallScale = { 1.0f, 1.0f, 1.0f };

// 初期化処理
void Ball::Init()
{
    m_Position = {0.0f, 3.0f, 0.0f};
    m_Rotation = {0.0f, 0.0f, 0.0f};

    //m_ModelRenderer = new ModelRenderer();
    //m_ModelRenderer->Load("asset\\model\\ball.fbx");

	m_AnimationModel = new AnimationModel();
	m_AnimationModel->Load("asset\\model\\ball.fbx");

    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

// 終了処理
void Ball::Uninit()
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
void Ball::Update()
{
    // 簡易物理演算
	const float DeltaTime = 1.0f / 60.0f;
	const Vector3 Gravity = { 0.0f, -9.8f, 0.0f };

	// 速度に重力を加算
	m_Velocity += Gravity * DeltaTime;
	m_Position += m_Velocity * DeltaTime;

	// 床での反発
    if (m_Position.y - m_Radius < 0.0f)
    {
        m_Position.y = m_Radius;
		m_Velocity.y = -m_Velocity.y * m_Bounce;

		// 摩擦っぽく減速
		m_Velocity.x *= 0.8f;
		m_Velocity.z *= 0.8f;
    }

	// 簡易壁反発
    if (m_Position.x > 9.5f - m_Radius) { m_Position.x = 9.5f - m_Radius; m_Velocity.x *= -m_Bounce; }
	if (m_Position.x < -9.5f + m_Radius) { m_Position.x = -9.5f + m_Radius; m_Velocity.x *= -m_Bounce; }
	if (m_Position.z > 14.5f - m_Radius) { m_Position.z = 14.5f - m_Radius; m_Velocity.z *= -m_Bounce; }
	if (m_Position.z < -14.5f + m_Radius) { m_Position.z = -14.5f + m_Radius; m_Velocity.z *= -m_Bounce; }
}

// 描画処理
void Ball::Draw()
{
    // 入力レイアウト
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // テクスチャ設定
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // マトリクス設定
    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
    rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);
	m_AnimationModel->Draw();
}