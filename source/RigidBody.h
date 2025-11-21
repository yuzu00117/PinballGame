#pragma once

#include "Component.h"
#include "Vector3.h"
#include "CollisionInfo.h"

/// <summary>
/// 剛体コンポーネント
/// 物理挙動を担当する。デフォルト衝突処理用
/// </summary>
class RigidBody : public Component
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 衝突時のデフォルト処理
    /// </summary>
    void ResolveCollision(const CollisionInfo& info);

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };          // 速度ベクトル
    Vector3 m_PreviousPosition = { 0.0f, 0.0f, 0.0f };  // 前フレームの位置
    float m_Restitution = 0.8f;                         // 反発係数 (1で完全反発、0で非反発)
    float m_Mass = 1.0f;                                // 質量（現在は未使用）
    bool m_UseGravity = true;                           // 重力の影響を受けるかどうか
    bool m_IsKinematic = false;                         // キネマティックフラグ (trueなら物理演算の影響を受けない)
    bool m_IsFirstUpdate = true;                        // 初回更新フラグ
};