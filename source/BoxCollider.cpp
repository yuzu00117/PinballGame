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

    // --- CCD試行 ---
    GameObject* owner     = s->m_Owner;
    RigidBody*  rigidBody = owner ? owner->GetComponent<RigidBody>() : nullptr;

    // CCDを使うかどうかの判定
    bool useCCD = false;
    Vector3 p0, p1;

    if (rigidBody && !rigidBody->m_IsKinematic)
    {
        // 球の中心の「前フレーム位置」と「今フレーム位置」
        p0 = rigidBody->m_PreviousPosition + s->m_center;
        p1 = owner->m_Transform.Position + s->m_center;

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

            // Box視点
            outB.self         = b;
            outB.other        = s;
            outB.normal       = -hit.normal;
            outB.penetration  = 0.0f;
            outB.contactPoint = contact;
            outB.isCCDHit    = true;

            // Sphere視点
            outS.self         = s;
            outS.other        = b;
            outS.normal       = hit.normal;
            outS.penetration  = 0.0f;      // CCDなのでめり込み無し
            outS.contactPoint = contact;
            outS.isCCDHit    = true;

            return true;
        }
    }

    // --- 通常の衝突判定 ---
    // 球の中心（ワールド）
    Vector3 centerW = s->GetWorldPosition();
    float radius = s->m_radius;

    // Box の OBB ワールド行列を分解（スケール／回転／平行移動）
    XMMATRIX obbWorld = b->GetWorldMatrix();

    XMVECTOR scaleV, rotQ, transV;
    XMMatrixDecompose(&scaleV, &rotQ, &transV, obbWorld);

    // 半径（ワールド単位）
    XMFLOAT3 scaleF;
    XMStoreFloat3(&scaleF, scaleV);

    // OBB の中心（ワールド）
    XMFLOAT3 centerF;
    XMStoreFloat3(&centerF, transV);
    Vector3 boxCenterW{centerF.x, centerF.y, centerF.z};

    // OBB の半径（各軸方向の半サイズ：ワールド単位）
    Vector3 half{
        0.5f * scaleF.x,
        0.5f * scaleF.y,
        0.5f * scaleF.z};

    // 回転行列（スケール抜き）
    XMMATRIX rotM = XMMatrixRotationQuaternion(rotQ);
    XMMATRIX invRotM = XMMatrixTranspose(rotM); // 回転行列の逆は転置

    // 球の中心を OBB ローカル空間へ変換
    XMVECTOR cW = XMVectorSet(centerW.x, centerW.y, centerW.z, 1.0f);
    XMVECTOR cRelW = XMVectorSubtract(cW, transV);         // 中心基準
    XMVECTOR cL = XMVector3TransformCoord(cRelW, invRotM); // ローカルへ

    XMFLOAT3 cf;
    XMStoreFloat3(&cf, cL);
    Vector3 centerL{cf.x, cf.y, cf.z};

    // ローカル空間で最近接点を求める
    Vector3 closestL;
    closestL.x = Clamp(centerL.x, -half.x, half.x);
    closestL.y = Clamp(centerL.y, -half.y, half.y);
    closestL.z = Clamp(centerL.z, -half.z, half.z);

    // 差分ベクトル（ローカル）
    Vector3 diffL = centerL - closestL;
    float dist = diffL.Length();

    // 離れていれば非衝突
    if (dist > radius)
    {
        return false;
    }

    // 法線（ローカル）
    Vector3 normalL;
    if (dist > 1e-6f)
    {
        normalL = diffL / dist;
    }
    else
    {
        // ほぼ中心にめり込んだら適当な法線
        normalL = {1.0f, 0.0f, 0.0f};
    }

    float penetration = radius - dist;

    // ローカル法線をワールド空間に変換
    XMVECTOR nL = XMVectorSet(normalL.x, normalL.y, normalL.z, 0.0f);
    XMVECTOR nW = XMVector3TransformNormal(nL, rotM);
    nW = XMVector3Normalize(nW);

    XMFLOAT3 nWf;
    XMStoreFloat3(&nWf, nW);
    Vector3 normalW{nWf.x, nWf.y, nWf.z};

    // 接触点（ローカル → ワールド）
    XMVECTOR closestLVec = XMVectorSet(closestL.x, closestL.y, closestL.z, 1.0f);
    XMVECTOR contactRelW = XMVector3TransformCoord(closestLVec, rotM);
    XMVECTOR contactWVec = XMVectorAdd(contactRelW, transV);

    XMFLOAT3 cWf;
    XMStoreFloat3(&cWf, contactWVec);
    Vector3 contactW{cWf.x, cWf.y, cWf.z};

    // Box視点
    outB.self = b;
    outB.other = s;
    outB.normal = -normalW; // Box は内向き（元実装と同じ向き）
    outB.penetration = penetration;
    outB.contactPoint = contactW;
    outB.isCCDHit = false;

    // Sphere視点
    outS.self = s;
    outS.other = b;
    outS.normal = normalW; // 球側は外向き
    outS.penetration = penetration;
    outS.contactPoint = contactW;
    outS.isCCDHit = false;

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