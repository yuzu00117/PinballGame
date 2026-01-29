#pragma once

#include "Collider.h"

/// <summary>
/// ボックスコライダークラス
/// HACK: マジックナンバー多用してるので要修正
/// </summary>
class BoxCollider : public Collider
{
public:
    // --- 変数定義 ---
    Vector3 Center = { 0.0f, 0.0f, 0.0f }; // ボックスの中心座標
    Vector3 Size = { 1.0f, 1.0f, 1.0f };   // ボックスのサイズ（幅・高さ・奥行き）
    
    // --- 関数定義 ---
    /// <summary>
    /// ワールド座標を返す
    /// </summary>
    Vector3 GetWorldPosition() const override
    {
        if (!m_Transform)
            return Center;

        const auto world = m_Transform->GetWorldMatrix();
        DirectX::XMFLOAT3 pos{};
        DirectX::XMStoreFloat3(
            &pos,
            DirectX::XMVector3TransformCoord(
                DirectX::XMVectorSet(Center.x, Center.y, Center.z, 1.0f), world));
        return Vector3{ pos.x, pos.y, pos.z };
    }

    /// <summary>
    /// ワールド行列を返す
    /// </summary>
    DirectX::XMMATRIX GetWorldMatrix() const;

    /// <summary>
    /// 衝突処理
    /// </summary>
    bool CheckCollision(Collider* other,
                        CollisionInfo& outSelf,
                        CollisionInfo& outOther) override;

    /// <summary>
    /// ワールド座標系でのAABBを取得する
    /// </summary>
    void GetWorldAABB(Vector3& outMin, Vector3& outMax) const;

    /// <summary>
    /// コライダーのデバッグ描画
    /// </summary>
    void DebugDraw() override;
};
