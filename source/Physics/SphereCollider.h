//------------------------------------------------------------------------------
// SphereCollider
//------------------------------------------------------------------------------
// 役割:
// 球体形状の当たり判定を提供する Collider。
// ローカル中心座標と半径を持ち、他 Collider との衝突判定を行う。
//
// 設計意図:
// 形状ごとの判定ロジックを派生クラスに分離し、
// Collider の共通仕様（Owner/Transform参照、イベント中継等）と分担する。
// SphereCollider は球体としての最小パラメータ（中心/半径）に責務を限定する。
//
// 構成:
// - ローカル中心           : m_center（ローカル空間）
// - 半径                   : m_radius
// - ワールド座標取得        : Transform を用いて中心を変換
// - 衝突判定               : CheckCollision（実装はcpp側）
// - デバッグ描画            : DebugDraw（任意）
//
// NOTE:
// m_center はローカル座標。ワールド中心は Transform で変換した結果。
// m_Transform / m_Owner は非所有参照であり、Owner 生存期間中のみ有効。
//------------------------------------------------------------------------------
#pragma once

#include "Collider.h"
#include "Vector3.h"
#include <cmath>

class BoxCollider;

/// 球体コライダー
/// - ローカル中心（m_center）と半径（m_radius）を持つ
/// - GetWorldPosition() は中心を Transform でワールドへ変換した値を返す
class SphereCollider : public Collider
{
public:
    // ----------------------------------------------------------------------
    // パラメータ
    // ----------------------------------------------------------------------
    Vector3 m_center = { 0.0f, 0.0f, 0.0f };  // 球のローカル中心座標
    float   m_radius = 5.0f;                  // 球の半径

public:
    // ----------------------------------------------------------------------
    // 位置・判定
    // ----------------------------------------------------------------------
    /// 球のワールド中心座標を取得する
    /// - m_Transform が未設定の場合はローカル中心（m_center）を返す
    /// NOTE: m_center はローカル値のため、Transform が設定されている前提では返値は「ワールド中心」になる
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

    /// 他の Collider との衝突判定を行う
    /// - outSelf  : 自身側の衝突情報
    /// - outOther : 相手側の衝突情報
    /// 戻り値    : 衝突している場合 true
    bool CheckCollision(
        Collider* other,
        CollisionInfo& outSelf,
        CollisionInfo& outOther) override;

    // ----------------------------------------------------------------------
    // デバッグ
    // ----------------------------------------------------------------------
    /// 球のデバッグ描画
    void DebugDraw() override;
};
