#include "main.h"
#include "renderer.h"
#include "MathUtil.h"
#include "GameObject.h"

// コライダー関連
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "ColliderUtility.h"
#include "RigidBody.h"

// ----------------------------------------------------------------------
// OBBヘルパー
// ----------------------------------------------------------------------
struct OBBData
{
    Vector3 center;     // 中心座標（ワールド）
    Vector3 axis[3];    // 各軸（ワールド、正規化済み）
    float   half[3];    // 各軸方向の半サイズ
};

// 3Dベクトルの内積
static float Dot3(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 3Dベクトルの外積
static Vector3 Cross3(const Vector3& a, const Vector3& b)
{
    return Vector3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// ベクトルの正規化（ゼロベクトル対策版）
static Vector3 NormalizeSafe(const Vector3& v)
{
    float lenSq = v.LengthSq();
    if (lenSq < 1e-12f) return { 1.0f, 0.0f, 0.0f };

    float invLen = 1.0f / sqrtf(lenSq);
    return v * invLen;
}

// Box から OBB データを取得する
static void BuildOBBFromBox(const BoxCollider* box, OBBData& out)
{
    XMMATRIX world = box->GetWorldMatrix();

    XMVECTOR scaleV, rotQ, transV;
    XMMatrixDecompose(&scaleV, &rotQ, &transV, world);

    XMFLOAT3 scaleF, transF;
    XMStoreFloat3(&scaleF, scaleV);
    XMStoreFloat3(&transF, transV);

    out.center = { transF.x, transF.y, transF.z };
    out.half[0] = 0.5f * fabsf(scaleF.x);
    out.half[1] = 0.5f * fabsf(scaleF.y);
    out.half[2] = 0.5f * fabsf(scaleF.z);

    XMMATRIX rotM = XMMatrixRotationQuaternion(rotQ);

    XMFLOAT4X4 rotF;
    XMStoreFloat4x4(&rotF, rotM);

    // DirectX は行ベクトルなので、行 0,1,2 を各軸として使う
    out.axis[0] = NormalizeSafe(Vector3{ rotF._11, rotF._12, rotF._13 }); // X軸
    out.axis[1] = NormalizeSafe(Vector3{ rotF._21, rotF._22, rotF._23 }); // Y軸
    out.axis[2] = NormalizeSafe(Vector3{ rotF._31, rotF._32, rotF._33 }); // Z軸
}

// ワールド→OBB ローカル変換
static Vector3 ToLocalPoint(const OBBData& obb, const Vector3& pW)
{
    Vector3 d = pW - obb.center;
    return Vector3{
        Dot3(d, obb.axis[0]),
        Dot3(d, obb.axis[1]),
        Dot3(d, obb.axis[2])
    };
}

// OBB ローカル→ワールド変換
static Vector3 ToWorldPoint(const OBBData& obb, const Vector3& pL)
{
    return obb.center +
           obb.axis[0] * pL.x +
           obb.axis[1] * pL.y +
           obb.axis[2] * pL.z;
}

// 球 vs OBB のオーバーラップ（静的判定）
static bool IsSphereOverlappingOBB(const Vector3& centerW, float radius, const OBBData& obb)
{
    Vector3 centerL = ToLocalPoint(obb, centerW);
    Vector3 half(obb.half[0], obb.half[1], obb.half[2]);

    Vector3 closestL;
    closestL.x = Clamp(centerL.x, -half.x, half.x);
    closestL.y = Clamp(centerL.y, -half.y, half.y);
    closestL.z = Clamp(centerL.z, -half.z, half.z);

    Vector3 diff = centerL - closestL;
    float distSq = diff.LengthSq();

    return distSq <= (radius * radius);
}

// 線分 (p0W→p1W) 上を動く Sphere 中心と OBB の CCD 判定
static bool IntersectSegmentSphereVsOBB(const Vector3& p0W, const Vector3& p1W,
                                        const OBBData& obb, float radius,
                                        CcdHit* outHit)
{
    const float EPS = 1e-6f;

    // 線分を OBB ローカル空間に変換
    Vector3 p0L = ToLocalPoint(obb, p0W);
    Vector3 p1L = ToLocalPoint(obb, p1W);
    Vector3 dL  = p1L - p0L;

    // 半径 r で膨らませた AABB（ローカル）
    Vector3 minE(-obb.half[0] - radius,
                 -obb.half[1] - radius,
                 -obb.half[2] - radius);
    Vector3 maxE(+obb.half[0] + radius,
                 +obb.half[1] + radius,
                 +obb.half[2] + radius);

    float tMin = 0.0f;
    float tMax = 1.0f;

    // 3軸のスラブとの交差を求める
    for (int i = 0; i < 3; ++i)
    {
        float origin = (i == 0) ? p0L.x : (i == 1 ? p0L.y : p0L.z);
        float dir    = (i == 0) ? dL.x  : (i == 1 ? dL.y  : dL.z);
        float minV   = (i == 0) ? minE.x : (i == 1 ? minE.y : minE.z);
        float maxV   = (i == 0) ? maxE.x : (i == 1 ? maxE.y : maxE.z);

        if (fabsf(dir) < EPS)
        {
            // 線分が軸にほぼ平行
            if (origin < minV || origin > maxV)
            {
                return false;
            }
        }
        else
        {
            float invD = 1.0f / dir;
            float t1 = (minV - origin) * invD;
            float t2 = (maxV - origin) * invD;

            if (t1 > t2)
            {
                float tmp = t1; t1 = t2; t2 = tmp;
            }

            if (t1 > tMin) tMin = t1;
            if (t2 < tMax) tMax = t2;

            if (tMin > tMax)
            {
                return false;
            }
        }
    }

    if (tMin < 0.0f || tMin > 1.0f)
    {
        return false;
    }

    float tHit = tMin;

    // ヒット時の球中心（ローカル）
    Vector3 centerL = p0L + dL * tHit;

    // 元の OBB（膨らませる前）の最近接点
    Vector3 half(obb.half[0], obb.half[1], obb.half[2]);
    Vector3 closestL;
    closestL.x = Clamp(centerL.x, -half.x, half.x);
    closestL.y = Clamp(centerL.y, -half.y, half.y);
    closestL.z = Clamp(centerL.z, -half.z, half.z);

    Vector3 diffL = centerL - closestL;
    float distSq = diffL.LengthSq();

    Vector3 normalL;
    if (distSq > EPS)
    {
        normalL = diffL / sqrtf(distSq); // Box → Sphere
    }
    else
    {
        normalL = Vector3{ 1.0f, 0.0f, 0.0f }; // 適当
    }

    // ローカル → ワールド
    Vector3 contactBoxW = ToWorldPoint(obb, closestL);
    Vector3 normalW =
          obb.axis[0] * normalL.x
        + obb.axis[1] * normalL.y
        + obb.axis[2] * normalL.z;
    normalW.Normalize();

    if (outHit)
    {
        outHit->point  = contactBoxW; // Box 表面の接触点
        outHit->normal = normalW;     // Box → Sphere
    }

    return true;
}

// ----------------------------------------------------------------------
// 衝突処理
// ----------------------------------------------------------------------
// Box vs Boxの衝突判定
static bool BoxVsBox(BoxCollider* a, BoxCollider* b,
                     CollisionInfo& outA, CollisionInfo& outB)
{
    const float EPS = 1e-6f;

    OBBData A, B;
    BuildOBBFromBox(a, A);
    BuildOBBFromBox(b, B);

    // B中心を Aローカル軸で見る
    Vector3 t = B.center - A.center;

    // t を A の各軸方向へ投影
    float tA[3] = {
        Dot3(t, A.axis[0]),
        Dot3(t, A.axis[1]),
        Dot3(t, A.axis[2])
    };

    // R[i][j] = Dot(A.axis[i], B.axis[j])
    float R[3][3];
    float AbsR[3][3];

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[i][j] = Dot3(A.axis[i], B.axis[j]);
            AbsR[i][j] = fabsf(R[i][j]) + EPS; // ゼロ除算防止の微小値
        }
    }

    float minOverlap = 1e30f;
    Vector3 bestNormal = { 1.0f, 0.0f, 0.0f };

    auto updateAxis = [&](const Vector3& axis, float dist, float ra, float rb)
    {
        float overlap = (ra + rb) - dist;
        if (overlap < 0.0f)
        {
            return false; // 分離軸が見つかった → 非衝突
        }

        if (overlap < minOverlap)
        {
            // t と axis の向きから法線方向を決定（ほぼ A→B）
            float sign = Dot3(axis, t) < 0.0f ? -1.0f : 1.0f;
            bestNormal = NormalizeSafe(axis * sign);
            minOverlap = overlap;
        }
        return true;
    };

    // --- 1. A の各軸 ---
    for (int i = 0; i < 3; ++i)
    {
        float ra = A.half[i];
        float rb = B.half[0] * AbsR[i][0] +
                   B.half[1] * AbsR[i][1] +
                   B.half[2] * AbsR[i][2];
        float dist = fabsf(tA[i]);
        if (!updateAxis(A.axis[i], dist, ra, rb))
        {
            return false;
        }
    }

    // --- 2. B の各軸 ---
    for (int j = 0; j < 3; ++j)
    {
        float ra = A.half[0] * AbsR[0][j] +
                   A.half[1] * AbsR[1][j] +
                   A.half[2] * AbsR[2][j];
        float rb = B.half[j];

        float dist = fabsf(Dot3(t, B.axis[j]));
        if (!updateAxis(B.axis[j], dist, ra, rb))
        {
            return false;
        }
    }

    // --- 3. 交差軸 Ai x Bj ---
    for (int i = 0; i < 3; ++i)
    {
        int i1 = (i + 1) % 3;
        int i2 = (i + 2) % 3;

        for (int j = 0; j < 3; ++j)
        {
            int j1 = (j + 1) % 3;
            int j2 = (j + 2) % 3;

            Vector3 axis = Cross3(A.axis[i], B.axis[j]);
            float axisLenSq = Dot3(axis, axis);
            if (axisLenSq < 1e-6f)
            {
                continue; // ほぼ平行
            }

            float ra =
                A.half[i1] * AbsR[i2][j] +
                A.half[i2] * AbsR[i1][j];
            float rb =
                B.half[j1] * AbsR[i][j2] +
                B.half[j2] * AbsR[i][j1];

            float dist = fabsf(tA[i2] * R[i1][j] - tA[i1] * R[i2][j]);

            if (!updateAxis(axis, dist, ra, rb))
            {
                return false;
            }
        }
    }

    // ここまで来たら全軸でオーバーラップ → 衝突あり
    Vector3 contact = (A.center + B.center) * 0.5f;

    // A視点（self = A）
    outA.self         = a;
    outA.other        = b;
    outA.normal       = -bestNormal;   // A を押し出す方向
    outA.penetration  = minOverlap;
    outA.contactPoint = contact;
    outA.isCCDHit     = false;

    // B視点（self = B）
    outB.self         = b;
    outB.other        = a;
    outB.normal       =  bestNormal;   // B を押し出す方向
    outB.penetration  = minOverlap;
    outB.contactPoint = contact;
    outB.isCCDHit     = false;

    return true;
}   

// Box vs Sphere の衝突判定（OBBベース + CCD）
static bool BoxVsSphere(BoxCollider* b, SphereCollider* s,
                        CollisionInfo& outB, CollisionInfo& outS)
{
    GameObject* owner = s->m_Owner;
    if (!owner) return false;

    // Box の OBB 情報
    OBBData obb;
    BuildOBBFromBox(b, obb);

    const float radius = s->m_radius;

    // --- 1. CCD 試行（動いている Sphere にだけ適用） ---
    RigidBody* rigidBody = owner->GetComponent<RigidBody>();
    bool usedCCD = false;

    if (rigidBody && !rigidBody->m_IsKinematic)
    {
        // 球中心の前フレーム位置＆今フレーム位置
        Vector3 p0 = rigidBody->m_PreviousPosition + s->m_center;
        Vector3 p1 = owner->m_Transform.Position + s->m_center;

        Vector3 delta   = p1 - p0;
        float   moveLen = delta.Length();
        float   ccdMinMove = radius * 0.25f; // 半径の1/4以上動いていたら CCD を考慮

        // 前フレーム位置が OBB 外部にあった場合のみ CCD を試行
        float judgeRadius = radius * 1.05f; // 少し余裕を持たせる
        bool wasOutside = !IsSphereOverlappingOBB(p0, judgeRadius, obb);

        if (moveLen >= ccdMinMove && wasOutside)
        {
            CcdHit hit;
            if (IntersectSegmentSphereVsOBB(p0, p1, obb, radius, &hit))
            {
                // contactPoint: Box 表面の接触点
                Vector3 contactW = hit.point;
                Vector3 normalW = hit.normal; // Box → Sphere

                // Box視点（self=Box）：CCD扱いだが位置は直さない
                outB.self = b;
                outB.other = s;
                outB.normal = -normalW; // Box から見て自分を押し出す
                outB.penetration = 0.0f;
                outB.contactPoint = contactW;
                outB.isCCDHit = false; // Box 側では通常解決扱い

                // Sphere視点（self=Sphere）：CCDヒット
                outS.self = s;
                outS.other = b;
                outS.normal = normalW;   // Sphere を押し出す方向
                outS.penetration = 0.0f; // 位置は RigidBody で決める
                outS.contactPoint = contactW;
                outS.isCCDHit = true;

                usedCCD = true;
            }
        }
    }

    if (usedCCD)
    {
        return true;
    }

    // --- 2. 通常の静的衝突判定（球 vs OBB） ---
    // Sphere の中心（ワールド）
    Vector3 centerW = owner->m_Transform.Position + s->m_center;

    // ローカル空間へ変換
    Vector3 centerL = ToLocalPoint(obb, centerW);
    Vector3 half(obb.half[0], obb.half[1], obb.half[2]);

    // 最近接点（ローカル）
    Vector3 closestL;
    closestL.x = Clamp(centerL.x, -half.x, half.x);
    closestL.y = Clamp(centerL.y, -half.y, half.y);
    closestL.z = Clamp(centerL.z, -half.z, half.z);

    // 差分ベクトル
    Vector3 diffL = centerL - closestL;
    float distSq = diffL.LengthSq();
    float rSq    = radius * radius;

    if (distSq > rSq)
    {
        return false;
    }

    float dist = sqrtf(std::max(distSq, 1e-12f));

    // ローカル法線（Box → Sphere）
    Vector3 normalL;
    if (dist > 1e-6f)
    {
        normalL = diffL / dist;
    }
    else
    {
        normalL = Vector3{ 0.0f, 1.0f, 0.0f };
    }

    float penetration = radius - dist;

    // ワールド法線
    Vector3 normalW =
          obb.axis[0] * normalL.x
        + obb.axis[1] * normalL.y
        + obb.axis[2] * normalL.z;
    normalW.Normalize();

    // 接触点（Box 表面）
    Vector3 contactW = ToWorldPoint(obb, closestL);

    // Box視点
    outB.self         = b;
    outB.other        = s;
    outB.normal       = -normalW;      // Box から見て自分を押し出す方向
    outB.penetration  = penetration;
    outB.contactPoint = contactW;
    outB.isCCDHit     = false;

    // Sphere視点
    outS.self         = s;
    outS.other        = b;
    outS.normal       =  normalW;      // Sphere を押し出す方向
    outS.penetration  = penetration;
    outS.contactPoint = contactW;
    outS.isCCDHit     = false;

    return true;
}

// ----------------------------------------------------------------------
// ワールド行列取得
// ----------------------------------------------------------------------
XMMATRIX BoxCollider::GetWorldMatrix() const
{
    // ローカル空間では[-0.5, +0.5]のユニットボックス
    // → Sizeでスケーリング、Centerで平行移動
    const XMMATRIX scale     = XMMatrixScaling(Size.x, Size.y, Size.z);
    const XMMATRIX translate = XMMatrixTranslation(Center.x, Center.y, Center.z);
    
    if (m_Transform)
    {
        // Transformのワールド行列
        const XMMATRIX world = m_Transform->GetWorldMatrix();
        return scale * translate * world;
    }
    else
    {
        return scale * translate;
    }
}

// ----------------------------------------------------------------------
// 当たり判定関連
// ----------------------------------------------------------------------
// 衝突処理
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

// ワールド座標系でのAABBを取得する
void BoxCollider::GetWorldAABB(Vector3& outMin, Vector3& outMax) const
{
    // まずOBBのワールド行列を取得
    const XMMATRIX worldMatrix = GetWorldMatrix();

    // ローカル空間での8頂点を計算
    static const XMFLOAT3 kLocalCorners[8] = 
    {
        {-0.5f,-0.5f,-0.5f},{+0.5f,-0.5f,-0.5f},
        {+0.5f,+0.5f,-0.5f},{-0.5f,+0.5f,-0.5f},
        {-0.5f,-0.5f,+0.5f},{+0.5f,-0.5f,+0.5f},
        {+0.5f,+0.5f,+0.5f},{-0.5f,+0.5f,+0.5f}
    };

    // 0番目で初期化
    XMVECTOR v = XMVector3Transform(XMLoadFloat3(&kLocalCorners[0]), worldMatrix);
    XMFLOAT3 f;
    XMStoreFloat3(&f, v);
    Vector3 minV(f.x, f.y, f.z);
    Vector3 maxV(f.x, f.y, f.z);

    // 残り7頂点を処理
    for (int i = 1; i < 8; ++i)
    {
        v = XMVector3Transform(XMLoadFloat3(&kLocalCorners[i]), worldMatrix);
        XMStoreFloat3(&f, v);

        if (f.x < minV.x) minV.x = f.x;
        if (f.y < minV.y) minV.y = f.y;
        if (f.z < minV.z) minV.z = f.z;

        if (f.x > maxV.x) maxV.x = f.x;
        if (f.y > maxV.y) maxV.y = f.y;
        if (f.z > maxV.z) maxV.z = f.z;
    }

    outMin = minV;
    outMax = maxV;
}

// ----------------------------------------------------------------------
// コライダーのデバッグ描画関連
// ----------------------------------------------------------------------
// ボックスのワイヤーフレーム描画
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
    // ワールド行列を取得
    const XMMATRIX WorldMatrix = GetWorldMatrix();

    // 目立つ色（半透明）? 好みで変更可
    DrawWireUnitBox(WorldMatrix, s_DebugColor);
}