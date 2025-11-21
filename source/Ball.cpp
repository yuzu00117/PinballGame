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
#include "RigidBody.h"

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

    // Rigidbodyコンポーネントの追加
    m_RigidBody = AddComponent<RigidBody>();
    m_RigidBody->m_Restitution = m_Bounce;    // 反発係数を設定
    m_RigidBody->m_UseGravity = true;         // 重力を有効化
    m_RigidBody->m_IsKinematic = false;       // キネマティック無効化 
}

// 終了処理
void Ball::Uninit()
{
    // コンポーネントの解放
    m_ModelRenderer = nullptr;
    m_ColliderGroup = nullptr;
    m_RigidBody = nullptr;
}

// 更新処理
void Ball::Update()
{
#ifndef NDEBUG
    // SPACEキーで発射
    if (Input::GetKeyTrigger(VK_SPACE))
    {
        m_RigidBody->m_Velocity.z += 12.0f;
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