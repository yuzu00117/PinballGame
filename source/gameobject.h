#pragma once  
#include "Transform.h"
#include "Collider.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "DebugSettings.h"
#include <memory>
#include <vector>
#include <type_traits>
    
class Collider;

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
    virtual void Update()
    {
        // コンポーネントの更新
        for (auto& Component : m_Components) Component->Update();
        // 子オブジェクトの更新
        for (auto& Child : m_Children) Child->Update();
    }

    // 描画処理
    virtual void Draw()
    {
        // コンポーネントの描画
        for (auto& Component : m_Components) Component->Draw();
        // 子オブジェクトの描画
        for (auto& Child : m_Children) Child->Draw();

        // デバッグ用コライダー描画
        if (g_EnableColliderDebugDraw)
        {
            for (auto& component : m_Components)
            {
                if (auto collider = dynamic_cast<Collider*>(component.get()))
                {
                    collider->DebugDraw();
                }
            }
        }
    }

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

        // ColliderやMeshRendererのTransformリンク
        if constexpr (std::is_base_of<Collider, T>::value)
            comp->m_Transform = &m_Transform;
        if constexpr (std::is_base_of<MeshRenderer, T>::value)
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
    /// 子オブジェクトを追加する
    /// </summary>
    GameObject* CreateChild()
    {
        auto child = std::make_unique<GameObject>();
        GameObject* ptr = child.get();
        AttachChild(std::move(child));
        return ptr;
    }

    template <class T, class... Args>
    T* CreateChild(Args&&... args) {
        static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = child.get();
        AttachChild(std::move(child));
        return ptr;
    }

    /// <summary>
    /// 子オブジェクトをアタッチする
    /// </summary>
    void AttachChild(std::unique_ptr<GameObject> child)
    {
        child->m_Parent = this;
        child->m_Transform.SetParent(&m_Transform);
        m_Children.push_back(std::move(child));
    }

    /// <summary>
    /// 子オブジェクトをデタッチする
    /// </summary>
    void DetachAllChildren()
    {
        for (auto& Child : m_Children)
        {
            Child->m_Transform.ClearParent();
            Child->m_Parent = nullptr;
        }
        m_Children.clear();
    }

    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    Transform m_Transform; // オブジェクトのTransform情報

protected:
    // -------------------------------------------------------------------------------
    // 変数定義
    // -------------------------------------------------------------------------------
    GameObject* m_Parent = nullptr;                         // 親オブジェクトへのポインタ
    std::vector<std::unique_ptr<Component>> m_Components;   // 所属するコンポーネントのリスト
    std::vector<std::unique_ptr<GameObject>> m_Children;    // 子オブジェクトのリスト
};
