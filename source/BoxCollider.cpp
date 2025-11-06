#include "BoxCollider.h"
#include <algorithm>
#include <cmath>

// 値をaからbの範囲にクランプする関数
// TODO: MathUtilに移動予定
static inline float ClampFloat(float v, float a, float b)
{
    return std::max(a, std::min(v, b));
}

// 汎用 Box vs Box / Box vs Sphere 対応予定
bool BoxCollider::OnCollision(Collider& other)
{
    if (other.m_ColliderType == ColliderType::Box)
    {
        // --- Box vs Box ---
        const BoxCollider& b = static_cast<const BoxCollider&>(other);
        Vector3 minA = Center - Size * 0.5f;
        Vector3 maxA = Center + Size * 0.5f;
        Vector3 minB = b.Center - b.Size * 0.5f;
        Vector3 maxB = b.Center + b.Size * 0.5f;

        bool hit =
            (minA.x <= maxB.x && maxA.x >= minB.x) &&
            (minA.y <= maxB.y && maxA.y >= minB.y) &&
            (minA.z <= maxB.z && maxA.z >= minB.z);

        return hit;
    }

    // TODO: SphereColliderなどが増えたらここに対応追加
    return false;
}
