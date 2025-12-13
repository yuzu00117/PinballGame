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
    // ----------------------------------------------------------------------
    // Transformの初期設定
    // ----------------------------------------------------------------------
    m_Transform.Position = { 8.0f, 1.0f, 7.0f };
    m_Transform.Rotation = { 0.0f, 0.0f, 0.0f };
    m_Transform.Scale = kDefaultBallScale;

    // ----------------------------------------------------------------------
    // ModelRendererコンポーネントの追加
    // ----------------------------------------------------------------------
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//ball.obj");

    // ----------------------------------------------------------------------
    // ColliderGroupコンポーネントの追加
    // ----------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();

    SphereCollider* sphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();
    sphereCollider->m_radius = m_BallRadius;        // 半径を設定

    // ----------------------------------------------------------------------
    // Rigidbodyコンポーネントの追加
    // ----------------------------------------------------------------------
    m_RigidBody = AddComponent<RigidBody>();
    m_RigidBody->m_Restitution = m_BallBounce; // 反発係数を設定
    m_RigidBody->m_UseGravity = true;          // 重力を有効化
    m_RigidBody->m_IsKinematic = false;        // キネマティック無効化

    // ピンボール用の重力設定
    const float g = 9.8f; // 重力加速度s

    // テーブルがZマイナス方向に傾いているイメージ
    const float tiltDeg = 65.0f;
    const float rad = tiltDeg * XM_PI / 180.0f;

    const float gy = -g * std::cosf(rad); // Y成分
    const float gz = -g * std::sinf(rad); // Z成分

    m_RigidBody->m_Gravity = Vector3(0.0f, gy, gz);
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
#if defined(_DEBUG)
    // デバッグ用：キーでボールに力を加える処理（今のままでOK）
    if (Input::GetKeyTrigger('W'))
    {
        m_RigidBody->m_Velocity.z += 12.0f;
    }
    if (Input::GetKeyTrigger('S'))
    {
        m_RigidBody->m_Velocity.z -= 12.0f;
    }
    if (Input::GetKeyTrigger('D'))
    {
        m_RigidBody->m_Velocity.x += 12.0f;
    }
    if (Input::GetKeyTrigger('A'))
    {
        m_RigidBody->m_Velocity.x -= 12.0f;
    }
#endif

    // コンポーネントの更新
    GameObject::Update();

    // ------------------------------------------------------
    // ボールの高さ制限
    // ------------------------------------------------------
    Vector3& pos = m_Transform.Position;

    // 下に抜けた場合
    if (pos.y < kTableMinY)
    {
        pos.y = kTableMinY;
        
        // 下向き速度はカット
        if (m_RigidBody->m_Velocity.y < 0.0f)
        {
            m_RigidBody->m_Velocity.y = 0.0f;
        }
    }

    // 上に飛び出した場合
    if (pos.y > kTableMaxY)
    {
        pos.y = kTableMaxY;

        // 上向き速度はカット
        if (m_RigidBody->m_Velocity.y > 0.0f)
        {
            m_RigidBody->m_Velocity.y = 0.0f;
        }
    }
}

// 描画処理
void Ball::Draw()
{
    // コンポーネントの描画
    GameObject::Draw();
}

// ボールをリセットする
void Ball::ResetBall()
{
    // ------------------------------------------------------
    // 位置を右上にリセット
    // ここでは (5.0f, 1.0f, 5.0f) に戻す
    // 必要ならフィールド中心の座標に変更してください
    // ------------------------------------------------------
    m_Transform.Position = { 8.0f, 1.0f, 7.0f };

    // ------------------------------------------------------
    // 速度リセット
    // Rigidbody と Ball の両方を念のためリセット
    // ------------------------------------------------------
    m_Velocity = { 0.0f, 0.0f, 0.0f };

    if (m_RigidBody)
    {
        m_RigidBody->m_Velocity = { 0.0f, 0.0f, 0.0f };
    }
}
