#pragma once  
#include "Transform.h"
#include "Collider.h"
#include "Component.h"
#include "MeshRenderer.h"
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
    // --- 関数定義 ---
    virtual ~GameObject() = default;

    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    virtual void Init() {}  
    virtual void Uninit() {}  
    virtual void Update()
    {
        // コンポーネントの更新
        for (auto& c : m_Components)
        {
            c->Update();
        }
    }

    // 描画処理
    virtual void Draw()
    {
        // コンポーネントの描画
        for (auto& c : m_Components)
        {
            c->Draw();
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

    // --- 変数定義 ---
    Transform m_Transform;                                      // オブジェクトのTransform情報
    std::vector<std::unique_ptr<Component>> m_Components;       // 所属するコンポーネントのリスト
};
