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
    void Update() override
    {
        if (isKinematic || !m_Owner) return;

        m_Owner->m_Transform.Position += velocity;
    }

    /// <summary>
    /// 衝突時のデフォルト処理
    /// </summary>
    void ResolveCollision(const CollisionInfo& info)
    {
        if (isKinematic || !m_Owner) return;

        // 1. めり込み分だけ位置を修正
        if (info.penetration > 0.0f)
        {
            m_Owner->m_Transform.Position += info.normal * info.penetration;
        }

        // 2. 反発ベクトルを計算して速度に反映
        //    vn < 0 の場合のみ反発処理を行う
        float vn = velocity.Dot(info.normal);
        if (vn < 0.0f)
        {
            Vector3 vNormal = info.normal * vn; // 法線成分
            Vector3 vTangent = velocity - vNormal; // 接線成分
            velocity = vTangent - vNormal * restitution;
        }
    }

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    Vector3 velocity = { 0.0f, 0.0f, 0.0f };    // 速度ベクトル
    float restitution = 0.8f;                   // 反発係数 (1で完全反発、0で非反発)
    bool isKinematic = false;                   // キネマティックフラグ (trueなら物理演算の影響を受けない)
};