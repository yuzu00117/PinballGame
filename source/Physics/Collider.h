#pragma once

#include "vector3.h"
#include "Transform.h"
#include "Component.h"
#include "CollisionInfo.h"
#include <memory>

class GameObject;

/// <summary>
/// コライダーの基底クラス
/// </summary>
class Collider : public Component
{
public:
    // ------------------------------------------------------------------------------
    // --- 関数定義
    // ------------------------------------------------------------------------------
    // デフォルトデストラクタ
    virtual ~Collider() = default;

    /// <summary>
    /// コライダーのワールド座標を返す
    /// </summary>
    virtual Vector3 GetWorldPosition() const
    {
        // BUG: 親Transformを考慮していないのでコライダーだけ位置がズレる。
        return m_Transform ? m_Transform->Position : Vector3{ 0.0f, 0.0f, 0.0f };
    }

    /// <summary>
    /// 他のコライダーとの衝突処理
    /// </summary>
    virtual bool CheckCollision(Collider* other,
                                CollisionInfo& outSelf,
                                CollisionInfo& outOther) = 0;  
    
    /// <summary>
    /// GameObjectに転送するイベント
    /// </summary>
    void InvokeOnCollisionEnter(const CollisionInfo& info);
    void InvokeOnCollisionStay(const CollisionInfo& info);
    void InvokeOnCollisionExit(const CollisionInfo& info);
    // / トリガー用イベント転送
    void InvokeOnTriggerEnter(const CollisionInfo& info);
    void InvokeOnTriggerStay(const CollisionInfo& info);
    void InvokeOnTriggerExit(const CollisionInfo& info);
    
    /// <summary>
    /// コライダーのデバッグ描画
    /// </summary>
    virtual void DebugDraw() {}

    // ------------------------------------------------------------------------------
    // --- 定数定義
    // ------------------------------------------------------------------------------
    static constexpr XMFLOAT4 s_DebugColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // デバッグ描画用の色（緑）
    
    // ------------------------------------------------------------------------------
    // --- 変数定義
    // ------------------------------------------------------------------------------
    Transform* m_Transform = nullptr;                       // コライダーが所属するオブジェクトのTransformポインタ
    GameObject* m_Owner = nullptr;                          // 所属するオブジェクトへのポインタ

    bool m_IsTrigger = false;                               // UnityのIsTriggerに相当するフラグ

};