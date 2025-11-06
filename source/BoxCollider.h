#pragma once

#include "Collider.h"

/// <summary>
/// ボックスコライダークラス
/// </summary>
class BoxCollider : public Collider
{
public:
    // --- 関数定義 ---
    /// <summary>
    /// ワールド座標を返す
    /// </summary>
    Vector3 GetWorldPosition() const override
    {
        return m_Transform ? m_Transform->Position + Center : Center;
    }

    /// <summary>
    /// 衝突処理
    /// </summary>
    bool OnCollision(Collider& other) override;

    // --- 変数定義 ---
    Vector3 Center = { 0.0f, 0.0f, 0.0f }; // ボックスの中心座標
    Vector3 Size = { 1.0f, 1.0f, 1.0f };   // ボックスのサイズ（幅・高さ・奥行き）
    float Restitution = 0.8f;              // 反発係数
};