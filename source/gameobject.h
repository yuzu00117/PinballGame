#pragma once  
#include "Transform.h"
#include "Collider.h"
#include "Component.h"
#include <memory>
#include <vector>
#include <type_traits>
    
class Collider;
// class MeshRenderer; // TODO: 将来的に実装予定

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
        // if (m_MeshRenderer)
        // {
        //     m_MeshRenderer->Draw(m_Transform.GetWorldMatrix());
        // }

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

        // ColliderやTransformを持つ場合は自動リンク
        if constexpr (std::is_base_of<Collider, T>::value)
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
    // std::unique_ptr<MeshRenderer> m_MeshRenderer = nullptr;  // TODO: 将来的に実装予定
    std::vector<std::unique_ptr<Component>> m_Components;       // 所属するコンポーネントのリスト
};
