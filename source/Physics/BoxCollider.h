//------------------------------------------------------------------------------
// BoxCollider
//------------------------------------------------------------------------------
// 役割:
// 直方体（ボックス）形状の当たり判定を提供する Collider。
// ローカル中心座標とサイズを持ち、ワールド空間での AABB を用いた
// 衝突判定を行う。
//
// 設計意図:
// SphereCollider と同様に、形状固有の判定ロジックを派生クラスに集約する。
// BoxCollider では「中心＋サイズ」という最小パラメータで表現し、
// Transform によるワールド変換は共通処理として分離する。
//
// 構成:
// - ローカル中心           : Center
// - サイズ（幅/高さ/奥行） : Size
// - ワールド中心取得        : GetWorldPosition
// - ワールド行列取得        : GetWorldMatrix
// - AABB 計算               : GetWorldAABB
// - 衝突判定               : CheckCollision
// - デバッグ描画            : DebugDraw
//
// NOTE:
// Center / Size はローカル空間の値。
// ワールド空間での当たり判定は Transform を介して計算される。
//------------------------------------------------------------------------------
#pragma once

#include "Collider.h"

/// ボックス（直方体）コライダー
/// - ローカル中心（Center）とサイズ（Size）を持つ
/// - ワールド空間では AABB として扱われる
class BoxCollider : public Collider
{
public:
    // ----------------------------------------------------------------------
    // パラメータ
    // ----------------------------------------------------------------------
    Vector3 Center = { 0.0f, 0.0f, 0.0f }; // ボックスのローカル中心座標
    Vector3 Size   = { 1.0f, 1.0f, 1.0f }; // ボックスのサイズ（幅・高さ・奥行）

public:
    // ----------------------------------------------------------------------
    // 位置・行列
    // ----------------------------------------------------------------------
    /// ボックスのワールド中心座標を取得する
    /// - m_Transform が未設定の場合はローカル中心（Center）を返す
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

    /// ボックスのワールド行列を取得する
    /// - Transform が未設定の場合は単位行列を返す想定
    DirectX::XMMATRIX GetWorldMatrix() const;

    // ----------------------------------------------------------------------
    // 衝突判定
    // ----------------------------------------------------------------------
    /// 他の Collider との衝突判定を行う
    /// - outSelf  : 自身側の衝突情報
    /// - outOther : 相手側の衝突情報
    /// 戻り値    : 衝突している場合 true
    bool CheckCollision(
        Collider* other,
        CollisionInfo& outSelf,
        CollisionInfo& outOther) override;

    /// ワールド空間での AABB を取得する
    /// - outMin : AABB の最小座標
    /// - outMax : AABB の最大座標
    /// NOTE:
    /// 回転を含む場合でも、ワールド空間で軸整合された AABB を返す
    void GetWorldAABB(Vector3& outMin, Vector3& outMax) const;

    // ----------------------------------------------------------------------
    // デバッグ
    // ----------------------------------------------------------------------
    /// ボックスのデバッグ描画
    void DebugDraw() override;
};