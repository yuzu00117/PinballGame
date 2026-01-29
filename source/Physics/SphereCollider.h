#pragma once

#include "Collider.h"
#include "Vector3.h"
#include <cmath>

class BoxCollider;

/// <summary>
/// スフィアコライダークラス
/// </summary>
class SphereCollider : public Collider
{
public:
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    Vector3 m_center = { 0.0f, 0.0f, 0.0f };  // スフィアのローカル中心座標
    float m_radius = 5.0f;                    // スフィアの半径
    
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ワールド座標を返す
    /// </summary>
    Vector3 GetWorldPosition() const override
    {
        if (!m_Transform)
            return m_center;

        const auto world = m_Transform->GetWorldMatrix();
        DirectX::XMFLOAT3 pos{};
        DirectX::XMStoreFloat3(
            &pos,
            DirectX::XMVector3TransformCoord(
                DirectX::XMVectorSet(m_center.x, m_center.y, m_center.z, 1.0f), world));
        return Vector3{ pos.x, pos.y, pos.z };
    }

    /// <summary>
    /// 衝突処理
    /// </summary>
    bool CheckCollision(Collider* other,
                        CollisionInfo& outSelf,
                        CollisionInfo& outOther) override;

    /// <summary>
    /// コライダーのデバッグ描画
    /// </summary>
    void DebugDraw() override;
};
