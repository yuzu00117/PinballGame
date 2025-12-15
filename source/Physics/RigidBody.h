#pragma once

#include "Component.h"
#include "Vector3.h"
#include "CollisionInfo.h"
#include <cstdint>

// -----------------------------------------------------------------------
// 軸フリーズフラグ
// -----------------------------------------------------------------------
    // TODO: どこに置くべきか悩む
    enum class FreezeFlags : uint32_t
    {
        None = 0,
        PosX = 1 << 0,
        PosY = 1 << 1,
        PosZ = 1 << 2,
        RotX = 1 << 3,
        RotY = 1 << 4,
        RotZ = 1 << 5,
    };

    inline FreezeFlags operator|(FreezeFlags a, FreezeFlags b)
    {
        return static_cast<FreezeFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
        );
    }

    inline FreezeFlags& operator|=(FreezeFlags& a, FreezeFlags b)
    {
        a = a | b;
        return a;
    }

    inline bool HasFlag(FreezeFlags value, FreezeFlags flag)
    {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

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
    void Update(float deltaTime) override;

    /// <summary>
    /// 衝突時のデフォルト処理
    /// </summary>
    void ResolveCollision(const CollisionInfo& info);

    /// <summary>
    /// 軸フリーズフラグの設定
    /// </summary>
    void SetFreezeFlags(FreezeFlags flags) { m_FreezeFlags = flags; }

    /// <summary>
    /// 軸フリーズフラグの取得
    /// </summary>
    FreezeFlags GetFreezeFlags() const { return m_FreezeFlags; }

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    Vector3 m_Velocity         = { 0.0f, 0.0f, 0.0f };  // 速度ベクトル
    Vector3 m_PreviousPosition = { 0.0f, 0.0f, 0.0f };  // 前フレームの位置
    Vector3 m_Gravity          = { 0.0f, -9.8f, 0.0f }; // 重力ベクトル
    float m_Restitution        = 0.8f;                  // 反発係数 (1で完全反発、0で非反発)
    float m_Mass               = 1.0f;                  // 質量（現在は未使用）
    bool m_UseGravity          = true;                  // 重力の影響を受けるかどうか
    bool m_IsKinematic         = false;                 // キネマティックフラグ (trueなら物理演算の影響を受けない)
    bool m_IsFirstUpdate       = true;                  // 初回更新フラグ

private:
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    FreezeFlags m_FreezeFlags = FreezeFlags::None;     // 軸フリーズフラグ
};