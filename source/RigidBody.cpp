#include "RigidBody.h"
#include "GameObject.h"

namespace
{
    constexpr float kDeltaTime = 1.0f / 60.0f;      // 仮の固定フレームレート
    const Vector3 kGravity = { 0.0f, -9.8f, 0.0f }; // 重力加速度
}

void RigidBody::Update()
{
    if (!m_Owner || m_IsKinematic) return;

    // CCD用に前フレームの位置を保存
    if (m_IsFirstUpdate)
    {
        // 初回更新時は現在位置を前フレーム位置として保存
        m_PreviousPosition = m_Owner->m_Transform.Position;
        m_IsFirstUpdate = false;
    }
    else
    {
        // 2回目以降は前フレーム位置を更新
        m_PreviousPosition = m_Owner->m_Transform.Position;

    }
   
    // 重力の影響を受ける場合、速度に重力加速度を加算
    if (m_UseGravity)
    {
        m_Velocity += kGravity * kDeltaTime;
    }

    // 位置を速度分だけ更新
    m_Owner->m_Transform.Position += m_Velocity * kDeltaTime;
}

void RigidBody::ResolveCollision(const CollisionInfo& info)
{
    if (!m_Owner || m_IsKinematic) return;

    if (info.isCCDHit)
    {
        float vn = m_Velocity.Dot(info.normal);
        if (vn < 0.0f)
        {
            // 反発後の法線速度
            float newVn = -vn * m_Restitution;

            // 小さい反発はカット
            const float kRestThreshold = 0.2f;
            if (newVn < kRestThreshold)
            {
                newVn = 0.0f;
            }

            // v_nをnewVnに置き換える
            float delta = newVn - vn;
            m_Velocity += info.normal * delta;
        }
        return;
    }

    // 静的めり込みの解決
    // 1. めり込みを解消するために位置を修正
    if (info.penetration > 0.0f)
    {
        m_Owner->m_Transform.Position += info.normal * info.penetration;
    }

    // 2. 速度の反射or停止
    float vn = m_Velocity.Dot(info.normal);
    if (vn < 0.0f)
    {
        // 反発後の法線速度
        float newVn = -vn * m_Restitution;

        // 小さい反発はカット
        const float kRestThreshold = 0.2f;
        if (newVn < kRestThreshold)
        {
            newVn = 0.0f;
        }

        // v_nをnewVnに置き換える
        float delta = newVn - vn;
        m_Velocity += info.normal * delta;
    }
}