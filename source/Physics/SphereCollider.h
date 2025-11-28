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
        return m_Transform ? m_Transform->Position + m_center : m_center;
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