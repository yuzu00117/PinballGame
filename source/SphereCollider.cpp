#include "SphereCollider.h"
#include "BoxCollider.h"
#include "renderer.h"
#include "MathUtil.h"
#include <algorithm>

// 衝突処理
bool SphereCollider::OnCollision(Collider& other)
{
    // --- Sphere vs Sphere ---
    if (other.m_ColliderType == ColliderType::Sphere)
    {
        const SphereCollider& s = static_cast<SphereCollider&>(other);

        Vector3 a = GetWorldPosition();
        Vector3 b = s.GetWorldPosition();

        float distSq =
            (a.x - b.x) * (a.x - b.x) +
            (a.y - b.y) * (a.y - b.y) +
            (a.z - b.z) * (a.z - b.z);

        float r = m_radius + s.m_radius;
        return distSq <= r * r;
    }

    // --- Sphere vs Box ---
    if (other.m_ColliderType == ColliderType::Box)
    {
        const BoxCollider& b = static_cast<BoxCollider&>(other);

        // Boxのmin / maxを計算
        Vector3 boxMin = b.Center - b.Size * 0.5f;
        Vector3 boxMax = b.Center + b.Size * 0.5f;

        // Sphereの中心
        Vector3 p = GetWorldPosition();

        // 最近接点をBoxにClamp
        float cx = Clamp(p.x, boxMin.x, boxMax.x);
        float cy = Clamp(p.y, boxMin.y, boxMax.y);
        float cz = Clamp(p.z, boxMin.z, boxMax.z);

        float dx = p.x - cx;
        float dy = p.y - cy;
        float dz = p.z - cz;

        float distSq = dx*dx + dy*dy + dz*dz;

        return distSq <= m_radius * m_radius;
    }
    return false;
}

// デバッグ用ワイヤーボックス描画
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