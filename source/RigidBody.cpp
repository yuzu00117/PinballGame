#include "RigidBody.h"
#include "GameObject.h"

namespace
{
    constexpr float kDeltaTime = 1.0f / 60.0f;      // 仮の固定フレームレート
    const Vector3 kGravity = { 0.0f, -9.8f, 0.0f }; // 重力加速度
}

void RigidBody::Update()
{
    if (!m_Owner || isKinematic) return;

    // 重力の影響を受ける場合、速度に重力加速度を加算
    if (useGravity)
    {
        velocity += kGravity * kDeltaTime;
    }

    // 位置を速度分だけ更新
    m_Owner->m_Transform.Position += velocity * kDeltaTime;
}

void RigidBody::ResolveCollision(const CollisionInfo& info)
{
    if (!m_Owner || isKinematic) return;

    // 1. めり込み分だけ位置を修正
    if (info.penetration > 0.0f)
    {
        m_Owner->m_Transform.Position += info.normal * info.penetration;
    }

    // 速度反射
    float vn = velocity.Dot(info.normal);
    if (vn < 0.0f)
    {
        Vector3 vNormal = info.normal * vn;
        Vector3 vTangent = velocity - vNormal;
        velocity = vTangent - vNormal * restitution;
    }
}