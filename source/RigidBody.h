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
    Vector3 velocity = { 0.0f, 0.0f, 0.0f };    // 速度ベクトル
    float restitution = 0.8f;                   // 反発係数 (1で完全反発、0で非反発)
    float mass = 1.0f;                          // 質量（現在は未使用）
    bool useGravity = true;                     // 重力の影響を受けるかどうか
    bool isKinematic = false;                   // キネマティックフラグ (trueなら物理演算の影響を受けない)
};