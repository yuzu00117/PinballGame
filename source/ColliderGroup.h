#pragma once
#include "Collider.h"
#include <vector>
#include <memory>

/// <summary>
/// コライダーグループクラス
/// 複数のコライダーをまとめて扱うためのクラス
/// </summary>
class ColliderGroup : public Collider
{
public:
    // --- 関数定義 ---
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override
    {
        // 自分にOwner / Transform が付いたあとでも、各コライダーに継承させる
        for (auto& c : colliders)
        {
            c->m_Transform = this->m_Transform;
            c->m_Owner = this->m_Owner;
        }
    }

    /// <summary>
    /// コライダーをグループに追加する
    /// </summary>
    template<typename T, typename... Args>
    T* AddCollider(Args&&... args)
    {
        auto c = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = c.get();

        // Transform / Owner を継承
        c->m_Transform = this->m_Transform;
        c->m_Owner = this->m_Owner;

        colliders.push_back(std::move(c));
        return ptr;
    }

    /// <summary>
    /// 衝突処理
    /// </summary>
    bool OnCollision(Collider& other) override
    {
        bool hit = false;
        for (auto& c : colliders)
            if (c->OnCollision(other)) hit = true;
        return hit;
    }

    // --- 変数定義 ---
    std::vector<std::unique_ptr<Collider>> colliders;           // コライダーのリスト
    ColliderGroup() { m_ColliderType = ColliderType::Group; }   // コライダーの種類をグループに設定
};
