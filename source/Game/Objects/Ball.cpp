#include "Ball.h"

// システム関連
#include <algorithm>
#include <Windows.h>

// 入力
#include "input.h"

// コンポーネント
#include "ModelRenderer.h"
#include "ColliderGroup.h"
#include "SphereCollider.h"
#include "RigidBody.h"

// ----------------------------------------------------------------------
// 初期化処理
// ----------------------------------------------------------------------
// - Transform の初期設定
// - ModelRenderer / ColliderGroup(+SphereCollider) / RigidBody を構築
// - ピンボール用の重力（テーブル傾き）を設定
void Ball::Init()
{
    // ----------------------------------------------------------------------
    // Transform の初期設定
    // ----------------------------------------------------------------------
    m_Transform.Position = kDefaultBallPosition;
    m_Transform.Rotation = { 0.0f, 0.0f, 0.0f };
    m_Transform.Scale    = kDefaultBallScale;

    // ----------------------------------------------------------------------
    // ModelRenderer を追加
    // ----------------------------------------------------------------------
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//ball.obj");

    // ----------------------------------------------------------------------
    // ColliderGroup + SphereCollider を追加
    // ----------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();

    SphereCollider* sphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();
    sphereCollider->m_radius = m_BallRadius;

    // ----------------------------------------------------------------------
    // RigidBody を追加（物理設定）
    // ----------------------------------------------------------------------
    m_RigidBody = AddComponent<RigidBody>();
    m_RigidBody->m_Restitution  = m_BallBounce;
    m_RigidBody->m_UseGravity   = true;
    m_RigidBody->m_IsKinematic  = false;

    // ----------------------------------------------------------------------
    // ピンボール用の重力設定
    // ----------------------------------------------------------------------
    const float g = 10.0f;           // 重力加速度
    const float tiltDeg = 85.0f;     // テーブル傾き（度）
    const float rad = tiltDeg * XM_PI / 180.0f;

    const float gy = -g * std::cosf(rad); // Y成分
    const float gz = -g * std::sinf(rad); // Z成分

    m_RigidBody->m_Gravity = Vector3(0.0f, gy, gz);
}

// ----------------------------------------------------------------------
// 終了処理
// ----------------------------------------------------------------------
// - GameObject が Component を unique_ptr で所有しているため、ここでは参照を切るのみ
void Ball::Uninit()
{
    m_ModelRenderer = nullptr;
    m_ColliderGroup = nullptr;
    m_RigidBody     = nullptr;
}

// ----------------------------------------------------------------------
// 更新処理
// ----------------------------------------------------------------------
// - デバッグ時のみキー入力で速度を直接加算する
// - Component 更新は GameObject::Update に委譲
// - ボールの高さ制限（Y）を適用し、制限方向の速度をカットする
void Ball::Update(float deltaTime)
{
#if defined(_DEBUG)
    // NOTE: デバッグ用の直書き操作。物理挙動の検証用途として現状維持。
    if (Input::GetKeyTrigger('W')) { m_RigidBody->m_Velocity.z += 12.0f; }
    if (Input::GetKeyTrigger('S')) { m_RigidBody->m_Velocity.z -= 12.0f; }
    if (Input::GetKeyTrigger('D')) { m_RigidBody->m_Velocity.x += 12.0f; }
    if (Input::GetKeyTrigger('A')) { m_RigidBody->m_Velocity.x -= 12.0f; }
#endif

    // Component 更新
    GameObject::Update(deltaTime);

    // ----------------------------------------------------------------------
    // 高さ制限（Y）
    // ----------------------------------------------------------------------
    Vector3& pos = m_Transform.Position;

    // 下に抜けた場合
    if (pos.y < kTableMinY)
    {
        pos.y = kTableMinY;

        // 下向き速度はカット
        if (m_RigidBody && m_RigidBody->m_Velocity.y < 0.0f)
        {
            m_RigidBody->m_Velocity.y = 0.0f;
        }
    }

    // 上に飛び出した場合
    if (pos.y > kTableMaxY)
    {
        pos.y = kTableMaxY;

        // 上向き速度はカット
        if (m_RigidBody && m_RigidBody->m_Velocity.y > 0.0f)
        {
            m_RigidBody->m_Velocity.y = 0.0f;
        }
    }
}

// ----------------------------------------------------------------------
// 描画処理
// ----------------------------------------------------------------------
// - 描画は Component に委譲（GameObject::Draw）
void Ball::Draw()
{
    GameObject::Draw();
}

// ----------------------------------------------------------------------
// ボールをリセットする
// ----------------------------------------------------------------------
// - 位置を初期位置へ戻す
// - 速度をゼロ化する（Ball側 / RigidBody側）
// NOTE: 現状は二重管理のため両方をクリアしている
void Ball::ResetBall()
{
    m_Transform.Position = kDefaultBallPosition;

    m_Velocity = { 0.0f, 0.0f, 0.0f };
    if (m_RigidBody)
    {
        m_RigidBody->m_Velocity = { 0.0f, 0.0f, 0.0f };
    }
}
