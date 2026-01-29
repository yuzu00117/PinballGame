//------------------------------------------------------------------------------
// Collider
//------------------------------------------------------------------------------
// 役割:
// 衝突判定を担当する Component の基底クラス。
// 他の Collider との当たり判定を行い、結果を GameObject にイベントとして通知する。
//
// 設計意図:
// 衝突形状（Box / Sphere など）ごとの差分は派生クラスに委譲し、
// Collider 自体は「判定の共通インターフェース」と
// 「GameObject へのイベント中継」に責務を限定する。
//
// 構成:
// - Transform 参照           : ワールド座標計算に使用（非所有）
// - 衝突判定インターフェース : CheckCollision（純粋仮想）
// - 衝突イベント中継         : OnCollision / OnTrigger 系を GameObject へ転送
// - デバッグ描画             : DebugDraw（任意実装）
//
// NOTE:
// Collider 自身は空間管理や衝突解決を行わない。
// それらは上位システムで制御する前提。
//------------------------------------------------------------------------------
#pragma once

#include "vector3.h"
#include "Transform.h"
#include "Component.h"
#include "CollisionInfo.h"
#include <memory>

class GameObject;

/// コライダーの基底クラス
/// - Component として GameObject に所属する
/// - Transform は非所有参照（Owner 側で管理）
/// - 実際の形状・判定ロジックは派生クラスで実装する
class Collider : public Component
{
public:
    /// 仮想デストラクタ（派生 Collider を安全に破棄するため）
    virtual ~Collider() = default;

    // ----------------------------------------------------------------------
    // 位置・判定
    // ----------------------------------------------------------------------
    /// コライダーのワールド座標を取得する
    /// - Transform が未設定の場合は (0,0,0) を返す
    /// NOTE: Transform の所有権は持たないため、参照が有効な期間のみ使用可能
    virtual Vector3 GetWorldPosition() const
    {
        if (!m_Transform)
            return Vector3{ 0.0f, 0.0f, 0.0f };

        const auto world = m_Transform->GetWorldMatrix();
        DirectX::XMFLOAT3 pos{};
        DirectX::XMStoreFloat3(
            &pos,
            DirectX::XMVector3TransformCoord(
                DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), world));
        return Vector3{ pos.x, pos.y, pos.z };
    }

    /// 他の Collider との衝突判定を行う
    /// - outSelf  : 自身側の衝突情報
    /// - outOther : 相手側の衝突情報
    /// 戻り値    : 衝突している場合 true
    /// NOTE: 実際の判定ロジックは派生クラスで実装する
    virtual bool CheckCollision(
        Collider* other,
        CollisionInfo& outSelf,
        CollisionInfo& outOther) = 0;

    // ----------------------------------------------------------------------
    // 衝突イベント中継
    // ----------------------------------------------------------------------
    /// GameObject へ衝突開始イベントを転送する
    void InvokeOnCollisionEnter(const CollisionInfo& info);
    /// GameObject へ衝突継続イベントを転送する
    void InvokeOnCollisionStay(const CollisionInfo& info);
    /// GameObject へ衝突終了イベントを転送する
    void InvokeOnCollisionExit(const CollisionInfo& info);

    /// GameObject へトリガー開始イベントを転送する
    void InvokeOnTriggerEnter(const CollisionInfo& info);
    /// GameObject へトリガー継続イベントを転送する
    void InvokeOnTriggerStay(const CollisionInfo& info);
    /// GameObject へトリガー終了イベントを転送する
    void InvokeOnTriggerExit(const CollisionInfo& info);

    // ----------------------------------------------------------------------
    // デバッグ
    // ----------------------------------------------------------------------
    /// コライダーのデバッグ描画
    /// NOTE: 描画は DebugSettings 等で有効な場合のみ呼ばれる想定
    virtual void DebugDraw() {}

public:
    // ----------------------------------------------------------------------
    // 定数
    // ----------------------------------------------------------------------
    /// デバッグ描画用の色（デフォルト：緑）
    static constexpr XMFLOAT4 s_DebugColor =
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

public:
    // ----------------------------------------------------------------------
    // 参照・状態
    // ----------------------------------------------------------------------
    Transform*  m_Transform = nullptr;   // 非所有：所属 GameObject の Transform
    GameObject* m_Owner     = nullptr;   // 非所有：所属する GameObject

    bool m_IsTrigger        = false;     // トリガーフラグ
};