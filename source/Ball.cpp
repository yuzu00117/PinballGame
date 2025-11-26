#include "main.h"
#include "renderer.h"
#include "ball.h"
#include "camera.h"
#include "manager.h"
#include "input.h"
#include <Windows.h>
#include "Flipper.h"

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
    m_Transform.Position = { 0.0f, 0.0f, 0.0f };
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
    sphereCollider->m_radius = m_Radius;        // 半径を設定

    // ----------------------------------------------------------------------
    // Rigidbodyコンポーネントの追加
    // ----------------------------------------------------------------------
    m_RigidBody = AddComponent<RigidBody>();
    m_RigidBody->m_Restitution = m_Bounce;    // 反発係数を設定
    m_RigidBody->m_UseGravity = true;         // 重力を有効化
    m_RigidBody->m_IsKinematic = false;       // キネマティック無効化

    // ピンボール用の重力設定
    const float g = 9.8f; // 重力加速度s

    // テーブルがZマイナス方向に傾いているイメージ
    const float tiltDeg = 30.0f;
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

void Ball::OnCollisionEnter(const CollisionInfo& info)
{
    if (!info.self || !info.other) return;
    if (!m_RigidBody) return;

    GameObject* selfOwner  = info.self->m_Owner;
    GameObject* otherOwner = info.other->m_Owner;

    // ボールのコライダー以外なら無視
    if (selfOwner != this && otherOwner != this) return;

    // Ball視点の法線（SphereVsBox では「Box→Sphere」方向）
    Vector3 n = info.normal;
    n = n.NormalizeSafe();

    // 現在の速度
    Vector3 v = m_RigidBody->m_Velocity;

    // 法線方向速度
    float vn = v.Dot(n);

    // 進行方向が法線に向いているときだけ反射
    if (vn < 0.0f)
    {
        // 鏡面反射 v' = v - 2*(v·n)*n
        Vector3 reflected = v - n * (2.0f * vn);

        // フリッパーっぽいパンチ力を付与する最低速度
        const float minSpeed = 250.0f;
        float speed = reflected.Length();

        if (speed < minSpeed)
        {
            reflected = reflected.NormalizeSafe() * minSpeed;
        }

        // 速度を上書き
        m_RigidBody->m_Velocity = reflected;
    }
}
