#include "main.h"
#include "renderer.h"
#include "ball.h"
#include "modelRenderer.h"
#include "camera.h"
#include "manager.h"
#include "input.h"
#include <Windows.h>

// デフォルトのボールスケール定義
const Vector3 Ball::DefaultBallScale = { 0.5f, 0.5f, 0.5f };

// 初期化処理
void Ball::Init()
{
    // Transformの初期設定
    m_Transform.Position = { 0.0f, 3.0f, 0.0f };
    m_Transform.Rotation = { 0.0f, 0.0f, 0.0f };
    m_Transform.Scale = DefaultBallScale;

    m_ModelRenderer = new ModelRenderer();
    m_ModelRenderer->Load("asset\\model\\ball.obj");


    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\bin\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\bin\\unlitTexturePS.cso");
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

    // ---- テーブルの疑似傾斜（XZ方向のごく小さい加速度）----
    // 例：手前(?Z)へ 0.90 m/s^2、ほんの少し右(+X)へ 0.10 m/s^2
    const Vector3 SlopeAccel = { 0.0f, 0.0f, -2.0f };
    // 転がり減衰（床に接地しているときの速度にかかる弱いブレーキ）
    const float RollingFriction = 0.985f;   // 0.98?0.995 あたりで微調整
    // 空中減衰（空中のときは極わずかに減速）
    const float AirDamping = 0.999f;        // 0.998?1.0

	// 速度に重力を加算
	m_Velocity += Gravity * DeltaTime;
    // 疑似傾斜を加算
    m_Velocity += SlopeAccel * DeltaTime;
    // 減衰処理
	m_Position += m_Velocity * DeltaTime;

    // SPACEキーで発射
    if (Input::GetKeyTrigger(VK_SPACE))
    {
        m_Velocity.z += 12.0f;
    }

	// 床での反発
    const bool OnFloor = (m_Position.y - m_Radius <= 0.0f);
    if (OnFloor)
    {
        m_Position.y = m_Radius;
		m_Velocity.y = -m_Velocity.y * m_Bounce;

		// 転がり減衰
        m_Velocity.x *= RollingFriction;
        m_Velocity.z *= RollingFriction;
    }
    else
    {
        // 空中減衰
        m_Velocity.x *= AirDamping;
        m_Velocity.z *= AirDamping;
    }

	// 簡易壁反発
    if (m_Position.x > 9.5f - m_Radius) { m_Position.x = 9.5f - m_Radius; m_Velocity.x *= -m_Bounce; }
	if (m_Position.x < -9.5f + m_Radius) { m_Position.x = -9.5f + m_Radius; m_Velocity.x *= -m_Bounce; }
	if (m_Position.z > 14.5f - m_Radius) { m_Position.z = 14.5f - m_Radius; m_Velocity.z *= -m_Bounce; }
	if (m_Position.z < -14.5f + m_Radius) { m_Position.z = -14.5f + m_Radius; m_Velocity.z *= -m_Bounce; }

    // フィールドとの衝突
    Field *field = nullptr;
    for (auto obj : Manager::GetGameObjects())
    {
        field = dynamic_cast<Field *>(obj);
        if (field)
            break;
    }
    if (field)
    {
        //field->ResolveBallCollision(m_Position, m_Velocity, m_Radius);
    }
    // バンパーとの衝突
    for (auto obj : Manager::GetGameObjects())
    {
        if (auto *b = dynamic_cast<Bumper *>(obj))
        {
            b->Resolve(m_Position, m_Velocity, m_Radius);
        }
        // フリッパーとの衝突
        if (auto *f = dynamic_cast<Flipper *>(obj))
        {
            f->Resolve(m_Position, m_Velocity, m_Radius, DeltaTime);
        }
    }
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
	m_ModelRenderer->Draw();
}