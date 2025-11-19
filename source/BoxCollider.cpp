#include "main.h"
#include "renderer.h"
#include "MathUtil.h"

// コライダー関連
#include "BoxCollider.h"
#include "SphereCollider.h"

// 汎用 Box vs Box / Box vs Sphere 対応予定
bool BoxCollider::OnCollision(Collider& other)
{
    if (auto* b = dynamic_cast<BoxCollider*>(&other))
    {
        // --- Box vs Box ---
        Vector3 minA = Center - Size * 0.5f;
        Vector3 maxA = Center + Size * 0.5f;
        Vector3 minB = b->Center - b->Size * 0.5f;
        Vector3 maxB = b->Center + b->Size * 0.5f;

        bool hit =
            (minA.x <= maxB.x && maxA.x >= minB.x) &&
            (minA.y <= maxB.y && maxA.y >= minB.y) &&
            (minA.z <= maxB.z && maxA.z >= minB.z);

        return hit;
    }

    // --- Box vs Sphere ---
    if (auto* s = dynamic_cast<SphereCollider*>(&other))
    {
        Vector3 center = s->GetWorldPosition();

        Vector3 boxMin = Center - Size * 0.5f;
        Vector3 boxMax = Center + Size * 0.5f;

        float cx = Clamp(center.x, boxMin.x, boxMax.x);
        float cy = Clamp(center.y, boxMin.y, boxMax.y);
        float cz = Clamp(center.z, boxMin.z, boxMax.z);

        float dx = center.x - cx;
        float dy = center.y - cy;
        float dz = center.z - cz;

        float distSq = dx*dx + dy*dy + dz*dz;

        return distSq <= s->m_radius * s->m_radius;
    }
    // TODO: SphereColliderなどが増えたらここに対応追加
    return false;
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