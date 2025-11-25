#include "RigidBody.h"
#include "GameObject.h"
#include "SphereCollider.h"

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

    Vector3& pos = m_Owner->m_Transform.Position;

    // --- 1. CCDヒット: 位置もここで直してから速度反射 ---
    if (info.isCCDHit)
    {
        // SphereColliderのCCDだけ対応
        if (auto* sphere = dynamic_cast<SphereCollider*>(info.self))
        {
            float radius = sphere->m_radius;
            const float kSlop = radius * 0.01f; // めり込み許容値

            // 球中心 = 接触点 + 法線方向に半径分（+少し押し出し）
            Vector3 newCenter = info.contactPoint + info.normal * (radius + kSlop);

            // Transform.Positionはローカル原点なので、centerオフセットを引く
            pos = newCenter - sphere->m_center;
        }

        // 速度反射
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

    // --- 2. 通常衝突: penetration分だけ押し戻し→速度反射 ---
    if (info.penetration > 0.0f)
    {
        // 位置修正
        pos += info.normal * info.penetration;
    }

    float vn = m_Velocity.Dot(info.normal);
    if (vn < 0.0f)
    {
        float newVn = -vn * m_Restitution;

        const float kRestThreshold = 0.2f;
        if (newVn < kRestThreshold)
        {
            newVn = 0.0f;
        }

        float delta = newVn - vn;
        m_Velocity += info.normal * delta;
    }
}