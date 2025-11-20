#include "main.h"
#include "renderer.h"
#include "MathUtil.h"

// コライダー関連
#include "BoxCollider.h"
#include "SphereCollider.h"

// ----------------------------------------------------------------------
// 衝突処理
// ----------------------------------------------------------------------
// Box vs Boxの衝突判定
static bool BoxVsBox(BoxCollider* a, BoxCollider* b,
                      CollisionInfo& outA, CollisionInfo& outB)
{
    Vector3 minA, maxA;
    Vector3 minB, maxB;
    a->GetWorldAABB(minA, maxA);
    b->GetWorldAABB(minB, maxB);

    float overLapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
    float overLapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
    float overLapZ = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

    if (overLapX <= 0.0f || overLapY <= 0.0f || overLapZ <= 0.0f)
        return false;

    // 一番浅いめり込みの軸を選ぶ
    float penetration = overLapX;
    Vector3 normal{ 1.0f, 0.0f, 0.0f };

    Vector3 centerA = (minA + maxA) * 0.5f;
    Vector3 centerB = (minB + maxB) * 0.5f;
    Vector3 diff = centerB - centerA;

    if (overLapY < penetration)
    {
        penetration = overLapY;
        normal = { 0, diff.y >= 0.0f ? 1.0f : -1.0f, 0 };
    }
    if (overLapZ < penetration)
    {
        penetration = overLapZ;
        normal = { 0, 0, diff.z >= 0.0f ? 1.0f : -1.0f };
    }
    if (penetration == overLapX)    
    {
        normal.x = diff.x >= 0.0f ? 1.0f : -1.0f;
    }

    Vector3 contact = (centerA + centerB) * 0.5f;

    outA.self         = a;
    outA.other        = b;
    outA.normal       = normal;
    outA.penetration  = penetration;
    outA.contactPoint = contact;

    outB.self         = b;
    outB.other        = a;
    outB.normal       = -normal;
    outB.penetration  = penetration;
    outB.contactPoint = contact;

    return true;
}        

// Box vs Sphereの衝突判定
static bool BoxVsSphere(BoxCollider* b, SphereCollider* s,
                        CollisionInfo& outB, CollisionInfo& outS)
{
    Vector3 boxMin, boxMax;
    b->GetWorldAABB(boxMin, boxMax);

    Vector3 center = s->GetWorldPosition();

    // 最近接点
    float cx = Clamp(center.x, boxMin.x, boxMax.x);
    float cy = Clamp(center.y, boxMin.y, boxMax.y);
    float cz = Clamp(center.z, boxMin.z, boxMax.z);
    Vector3 closest{ cx, cy, cz };

    Vector3 diff = center - closest;
    float dist = diff.Length();
    float radius = s->m_radius;

    if (dist > radius)
        return false;

    // 衝突情報の設定
    Vector3 normal = (dist > 0.0001f) ? (diff / dist) : Vector3(0, 1, 0);
    float penetration = radius - dist;
    Vector3 contact = closest;

    // Box視点
    outB.self         = b;
    outB.other        = s;
    outB.normal       = -normal;
    outB.penetration  = penetration;
    outB.contactPoint = contact;

    // Sphere視点
    outS.self         = s;
    outS.other        = b;
    outS.normal       = normal;
    outS.penetration  = penetration;
    outS.contactPoint = contact;

    return true;
}

static void DrawWireUnitBox(const XMMATRIX& worldMatrix, const XMFLOAT4& color)
{
    // ボックスの頂点を定義
    const XMFLOAT3 p[8] = {
        {-0.5f,-0.5f,-0.5f},{+0.5f,-0.5f,-0.5f},{+0.5f,+0.5f,-0.5f},{-0.5f,+0.5f,-0.5f},
        {-0.5f,-0.5f,+0.5f},{+0.5f,-0.5f,+0.5f},{+0.5f,+0.5f,+0.5f},{-0.5f,+0.5f,+0.5f}
    };

    // ワイヤーフレームのエッジを定義
    const uint16_t e[24] = {
        0,1, 1,2, 2,3, 3,0, // 下
        4,5, 5,6, 6,7, 7,4, // 上
        0,4, 1,5, 2,6, 3,7  // 柱
    };

    DebugLineVertex v[24];

    // 位置をworld行列で変換して頂点バッファにセット
    for (int i = 0; i < 24; ++i)
    {
        XMVECTOR VertexPosition = XMVector3Transform(XMLoadFloat3(&p[e[i]]), worldMatrix);
        XMStoreFloat3(&v[i].Position, VertexPosition);
        v[i].Color = color;
    }

    // 描画
    Renderer::DrawDebugLines(v, 24);
}

// コライダーのデバッグ描画
void BoxCollider::DebugDraw()
{
    // ワールド行列を作成
    const XMMATRIX ScaleMatrix = XMMatrixScaling(Size.x, Size.y, Size.z);
    const XMMATRIX TransformMatrix = XMMatrixTranslation(Center.x, Center.y, Center.z);
    const XMMATRIX WorldMatrix = (m_Transform ? (ScaleMatrix * TransformMatrix * m_Transform->GetWorldMatrix()) : (ScaleMatrix * TransformMatrix));

    // 目立つ色（半透明）? 好みで変更可
    DrawWireUnitBox(WorldMatrix, s_DebugColor);
}

// ワールド座標系でのAABBを取得する
void BoxCollider::GetWorldAABB(Vector3& outMin, Vector3& outMax) const
{
    // ワールド中心
    Vector3 worldCenter = Center;
    Vector3 worldSize = Size;

    if (m_Transform)
    {
        worldCenter = m_Transform->Position + Center;

        worldSize = 
        {
            Size.x * m_Transform->Scale.x,
            Size.y * m_Transform->Scale.y,
            Size.z * m_Transform->Scale.z
        };
    }

    Vector3 half = worldSize * 0.5f;
    outMin = worldCenter - half;
    outMax = worldCenter + half;
}

bool BoxCollider::CheckCollision(Collider* other,
                                 CollisionInfo& outSelf,
                                 CollisionInfo& outOther)
{
    // --- Box vs Box ---
    if (auto* box = dynamic_cast<BoxCollider*>(other))
    {
        return BoxVsBox(this, box, outSelf, outOther);
    }

    // --- Box vs Sphere ---
    if (auto* sphere = dynamic_cast<SphereCollider*>(other))
    {
        return BoxVsSphere(this, sphere, outSelf, outOther);
    }
    return false;
}