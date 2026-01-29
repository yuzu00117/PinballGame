#include "RigidBody.h"
#include "GameObject.h"
#include "SphereCollider.h"

void RigidBody::Update(float deltaTime)
{
    if (!m_Owner || m_IsKinematic) return;

    // Tranformへの参照
    Transform& transform = m_Owner->m_Transform;

    // フリーズ前の位置・回転を保存
    const Vector3 originalPos = transform.Position;
    const Vector3 originalRot = transform.Rotation;

    // CCD用に前フレームの位置を保存
    if (m_IsFirstUpdate)
    {
        // 初回更新時は現在位置を前フレーム位置として保存
        m_PreviousPosition = transform.Position;
        m_IsFirstUpdate = false;
    }
    else
    {
        // 2回目以降は前フレーム位置を更新
        m_PreviousPosition = transform.Position;
    }

    // ----------------------------------------------------------------------
    // 重力適用
    //  ・m_Gravityは任意方向
    //  ・フリーズされている軸成分は、0にしてから加算
    // ---------------------------------------------------------------------- 
    if (m_UseGravity)
    {
        Vector3 gravity = m_Gravity;

        if (HasFlag(m_FreezeFlags, FreezeFlags::PosX) ) gravity.x = 0.0f;
        if (HasFlag(m_FreezeFlags, FreezeFlags::PosY) ) gravity.y = 0.0f;
        if (HasFlag(m_FreezeFlags, FreezeFlags::PosZ) ) gravity.z = 0.0f;

        m_Velocity += gravity * deltaTime;
    }

    // 位置を速度分だけ更新
    transform.Position += m_Velocity * deltaTime;
    // ----------------------------------------------------------------------
    // 軸フリーズ適用
    //  ・位置は更新前の値に戻す
    //  ・該当軸の速度を0にする
    // ----------------------------------------------------------------------
    if (HasFlag(m_FreezeFlags, FreezeFlags::PosX))
    {
        transform.Position.x = originalPos.x;
        m_Velocity.x = 0.0f;
    }
    if (HasFlag(m_FreezeFlags, FreezeFlags::PosY))
    {
        transform.Position.y = originalPos.y;
        m_Velocity.y = 0.0f;
    }
    if (HasFlag(m_FreezeFlags, FreezeFlags::PosZ))
    {
        transform.Position.z = originalPos.z;
        m_Velocity.z = 0.0f;
    }

    // 現状では角速度を持っていないので、回転フリーズは
    // 「他の処理で回転を変化させた場合に元に戻す」だけ実装
    if (HasFlag(m_FreezeFlags, FreezeFlags::RotX))
    {
        transform.Rotation.x = originalRot.x;
        // TODO: 将来角速度を実装したらここで0にする
    }
    if (HasFlag(m_FreezeFlags, FreezeFlags::RotY))
    {
        transform.Rotation.y = originalRot.y;
    }
    if (HasFlag(m_FreezeFlags, FreezeFlags::RotZ))
    {
        transform.Rotation.z = originalRot.z;
    }
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