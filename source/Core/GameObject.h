#pragma once
#include "DebugSettings.h"
#include "CollisionInfo.h"
#include <memory>
#include <vector>
#include <type_traits>

// コンポーネント関連ヘッダ
#include "Transform.h"
#include "Collider.h"
#include "Component.h"
#include "MeshRenderer.h"

// 前方宣言
class Collider;
class AnimationModel;

/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject  
{
public:
    // ------------------------------------------------------------------------------
    // 関数定義
    // ------------------------------------------------------------------------------
    /// <summary>
    /// デフォルトコンストラクタ・デストラクタ
    /// </summary>
    virtual ~GameObject() = default;

    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    virtual void Init() {}  
    virtual void Uninit() {}  
    virtual void Update();
    virtual void Draw();

    /// <summary>
    /// 任意のComponentを追加する
    /// </summary>
    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T *ptr = comp.get();

        // GameObjectとの連携を設定
        comp->m_Owner = this;

        // Collider / MeshRenderer / AnimationModel のときだけ Transform をリンク
        // 全ComponentsがTransformを持つわけではないため、ここで条件分岐する
        // ifが増え過ぎたら別途メソッド化してもよい
        // Collider
        if constexpr (std::is_base_of<Collider, T>::value)
            comp->m_Transform = &m_Transform;
        // MeshRenderer
        if constexpr (std::is_base_of<MeshRenderer, T>::value)
            comp->m_Transform = &m_Transform;
        // AnimationModel
        if constexpr (std::is_base_of<AnimationModel, T>::value)
            comp->m_Transform = &m_Transform;

        comp->Init(); // 初期化呼び出し
        m_Components.push_back(std::move(comp));
        return ptr;
    }

    /// <summary>
    /// 指定タイプのComponentを取得する（例：GetComponent<BoxCollider>()）
    /// </summary>
    template <typename T>
    T *GetComponent()
    {
        for (auto &c : m_Components)
            if (auto ptr = dynamic_cast<T *>(c.get()))
                return ptr;
        return nullptr;
    }

    /// <summary>
    /// 子オブジェクトを追加する（基本版）
    /// </summary>
    GameObject* CreateChild();

    /// <summary>
    /// 子オブジェクトを追加する（派生クラス対応版）
    /// </summary>
    template <class T, class... Args>
    T* CreateChild(Args&&... args) {
        static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = child.get();
        AttachChild(std::move(child));
        return ptr;
    }

    /// <summary>
    /// 既存のGameObjectを子オブジェクトとしてアタッチする
    /// </summary>
    void AttachChild(std::unique_ptr<GameObject> child);

    /// <summary>
    /// すべての子オブジェクトをデタッチする
    /// </summary>
    void DetachAllChildren();

    /// <summary>
    /// 衝突イベントコールバック
    /// </summary>
    virtual void OnCollisionEnter(const CollisionInfo& info) {}
    virtual void OnCollisionStay(const CollisionInfo& info) {}
    virtual void OnCollisionExit(const CollisionInfo& info) {}

    /// <summary>
    /// 子オブジェクトも含めて全てのコライダーを収集する
    /// </summary>
    void CollectCollidersRecursive(std::vector<Collider*>& outColliders);

    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    Transform m_Transform;                                  // オブジェクトのTransform情報

protected:
    // -------------------------------------------------------------------------------
    // 変数定義
    // -------------------------------------------------------------------------------
    GameObject* m_Parent = nullptr;                         // 親オブジェクトへのポインタ
    std::vector<std::unique_ptr<Component>> m_Components;   // 所属するコンポーネントのリスト
    std::vector<std::unique_ptr<GameObject>> m_Children;    // 子オブジェクトのリスト
};
