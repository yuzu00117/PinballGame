#include "main.h"
#include "renderer.h"
#include "ball.h"
#include "camera.h"
#include "manager.h"
#include "input.h"
#include <Windows.h>

// コンポーネント
#include "ModelRenderer.h"
#include "ColliderGroup.h"
#include "SphereCollider.h"

// 初期化処理
void Ball::Init()
{
    // Transformの初期設定
    m_Transform.Position = { 0.0f, 3.0f, 0.0f };
    m_Transform.Rotation = { 0.0f, 0.0f, 0.0f };
    m_Transform.Scale = kDefaultBallScale;

    // ModelRendererコンポーネントの追加
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//ball.obj");

    // ColliderGroupコンポーネントの追加
    m_ColliderGroup = AddComponent<ColliderGroup>();

    SphereCollider* sphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();
    sphereCollider->m_radius = m_Radius;        // 半径を設定
    sphereCollider->m_restitution = m_Bounce;   // 反発係数を設定
}

// 終了処理
void Ball::Uninit()
{
    // コンポーネントの解放
    m_ModelRenderer = nullptr;
    m_ColliderGroup = nullptr;
}

// 更新処理
void Ball::Update()
{
	const float deltaTime = 1.0f / 60.0f;
	const Vector3 gravity = { 0.0f, -9.8f, 0.0f };

    // 速度に重力を加算
    m_Velocity += gravity * deltaTime;

    // 位置更新
    m_Transform.Position += m_Velocity * deltaTime;

#ifndef NDEBUG
    // SPACEキーで発射
    if (Input::GetKeyTrigger(VK_SPACE))
    {
        m_Velocity.z += 12.0f;
    }
#endif

    // コンポーネントの更新
    GameObject::Update();
}

// 描画処理
void Ball::Draw()
{
    // コンポーネントの描画
    GameObject::Draw();
}