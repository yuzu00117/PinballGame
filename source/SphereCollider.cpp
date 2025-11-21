#include "main.h"
#include "MathUtil.h"
#include "renderer.h"
#include "GameObject.h"

// コライダー・物理関連
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "ColliderUtility.h"
#include "RigidBody.h"

// ----------------------------------------------------------------------
// 衝突処理
// ----------------------------------------------------------------------
// Sphere vs Sphereの衝突判定
static bool SphereVsSphere(SphereCollider* a, SphereCollider* b,
                            CollisionInfo& outA, CollisionInfo& outB)
{
    Vector3 posA = a->GetWorldPosition();
    Vector3 posB = b->GetWorldPosition();

    Vector3 diff = posB - posA;
    float dist = diff.Length();
    float rA = a->m_radius;
    float rB = b->m_radius;
    float rSum = rA + rB;

    if (dist > rSum)
        return false;

    Vector3 normalA;

    if (dist > 0.0001f)
        normalA = diff / dist;
    else
        normalA = Vector3(0.0f, 1.0f, 0.0f); // 適当な法線

    float penetration = rSum - dist;

    Vector3 contact = posA + normalA * (rA - penetration * 0.5f);

    // A視点
    outA.self = a;
    outA.other = b;
    outA.normal = normalA;
    outA.penetration = penetration;
    outA.contactPoint = contact;

    // B視点
    outB.self = b;
    outB.other = a;
    outB.normal = -normalA;
    outB.penetration = penetration;
    outB.contactPoint = contact;

    return true;
}

// Sphere vs Boxの衝突判定（CCD対応）
static bool SphereVsBox(SphereCollider* s, BoxCollider* b,
                        CollisionInfo& outS, CollisionInfo& outB)
{
    Vector3 boxMin, boxMax;
    b->GetWorldAABB(boxMin, boxMax);

    // --- CCD試行 ---
    GameObject* owner     = s->m_Owner;
    RigidBody*  rigidBody = owner ? owner->GetComponent<RigidBody>() : nullptr;

    // CCDを使うかどうかの判定
    bool useCCD = false;
    Vector3 p0, p1;

    if (rigidBody && !rigidBody->m_IsKinematic)
    {
        // 球の中心の「前フレーム位置」と「今フレーム位置」
        p0 = rigidBody->m_PreviousPosition + s->m_center;   // 前フレーム中心
        p1 = owner->m_Transform.Position + s->m_center;     // 今フレーム中心

        Vector3 delta = p1 - p0;
        float moveLen = delta.Length();
        float ccdMinMove = s->m_radius * 0.25f; // 半径の1/4以上動いていたらCCDを使う

        // NOTE: CCD は「今フレームの位置p1がBoxに近いとき」にだけ使う。
        // p0基準にすると接地スライド時のブルブルが増えたため、
        // 実用上 p1 で判定する実装を採用している。
        if (moveLen >= ccdMinMove && 
            IsSphereOverlappingBox(p1, s->m_radius, boxMin, boxMax))
        {
            useCCD = true;
        }
    }

    if (useCCD)
    {
        CcdHit hit;
        if (IntersectSegmentExpandedAABB(p0, p1, boxMin, boxMax, s->m_radius, &hit))
        {
            // 衝突位置まで戻す（少しだけ離す）
            const float kSlop = s->m_radius * 0.01f;
            Vector3 hitCenter = hit.point + hit.normal * kSlop;

            // GameObjectのPositionはローカル原点なので、中心オフセットを引く
            owner->m_Transform.Position = hitCenter - s->m_center;

            // 接触点（ボックス表面）
            Vector3 contact = hitCenter - hit.normal * s->m_radius;

            // Sphere視点
            outS.self         = s;
            outS.other        = b;
            outS.normal       = hit.normal;
            outS.penetration  = 0.0f;      // CCDなのでめり込み無し
            outS.contactPoint = contact;
            outS.isCCDHit    = true;

            // Box視点
            outB.self         = b;
            outB.other        = s;
            outB.normal       = -hit.normal;
            outB.penetration  = 0.0f;
            outB.contactPoint = contact;
            outB.isCCDHit    = true;

            return true;
        }
    }

    // --- CCDで当たらなかった場合は従来の静的判定にフォールバック ---

    Vector3 center = s->GetWorldPosition();

    // 最近接点
    float cx = Clamp(center.x, boxMin.x, boxMax.x);
    float cy = Clamp(center.y, boxMin.y, boxMax.y);
    float cz = Clamp(center.z, boxMin.z, boxMax.z);
    Vector3 closest{ cx, cy, cz };

    Vector3 diff = center - closest;
    float   dist = diff.Length();

    if (dist > s->m_radius)
        return false;

    Vector3 normal      = (dist > 0.0001f) ? (diff / dist) : Vector3(0, 1, 0);
    float   penetration = s->m_radius - dist;
    Vector3 contact     = closest;

    // Sphere視点
    outS.self         = s;
    outS.other        = b;
    outS.normal       = normal;
    outS.penetration  = penetration;
    outS.contactPoint = contact;

    // Box視点
    outB.self         = b;
    outB.other        = s;
    outB.normal       = -normal;
    outB.penetration  = penetration;
    outB.contactPoint = contact;

    return true;
}

// 衝突処理
bool SphereCollider::CheckCollision(Collider* other,
                                    CollisionInfo& outSelf,
                                    CollisionInfo& outOther)
{
    if (auto* sphere = dynamic_cast<SphereCollider*>(other))
    {
        return SphereVsSphere(this, sphere, outSelf, outOther);
    }
    else if (auto* box = dynamic_cast<BoxCollider*>(other))
    {
        return SphereVsBox(this, box, outSelf, outOther);
    }

    return false;
}

// ----------------------------------------------------------------------
// デバッグ用ワイヤーボックス描画
// ----------------------------------------------------------------------
static void DrawWireUnitBox(const XMMATRIX& worldMatrix, const XMFLOAT4& color)
{
    const int segments = 32;
    DebugLineVertex v[segments * 3 * 2];
    int index = 0;

    auto addLine = [&](const XMFLOAT3& a, const XMFLOAT3& b)
    {
        XMStoreFloat3(&v[index].Position, XMVector3Transform(XMLoadFloat3(&a), worldMatrix));
        v[index].Color = color;
        index++;

        XMStoreFloat3(&v[index].Position, XMVector3Transform(XMLoadFloat3(&b), worldMatrix));
        v[index].Color = color;
        index++;
    };

    for (int i = 0; i < segments; ++i)
    {
        float t0 = XM_2PI * (i / (float)segments);
        float t1 = XM_2PI * ((i + 1) / (float)segments);

        // XY平面
        addLine({ cosf(t0), sinf(t0), 0.0f }, { cosf(t1), sinf(t1), 0.0f });
        // YZ平面
        addLine({ 0.0f, cosf(t0), sinf(t0) }, { 0.0f, cosf(t1), sinf(t1) });
        // ZX平面
        addLine({ sinf(t0), 0.0f, cosf(t0) }, { sinf(t1), 0.0f, cosf(t1) });
    }
    Renderer::DrawDebugLines(v, index);
}

// コライダーのデバッグ描画
void SphereCollider::DebugDraw()
{
    const XMMATRIX S = XMMatrixScaling(m_radius, m_radius, m_radius);
    const XMMATRIX T = XMMatrixTranslation(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z);
    const XMMATRIX W = S * T;

    DrawWireUnitBox(W, s_DebugColor);
}