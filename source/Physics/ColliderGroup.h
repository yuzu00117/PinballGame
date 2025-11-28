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
    /// グループとしての当たり判定処理
    /// </summary>
    bool CheckCollision(Collider* other,
                        CollisionInfo& outSelf,
                        CollisionInfo& outOther) override
    {
        bool hit = false;

        for (auto& c : colliders)
        {
            CollisionInfo childself;
            CollisionInfo childother;

            if (c->CheckCollision(other, childself, childother))
            {
                if (!hit)
                {
                    // 最初の衝突時に outSelf / outOther を初期化
                    outSelf = childself;
                    outOther = childother;
                    hit = true;
                }
            }
        }
        
        if (hit)
        {
            // selfは「グループ」として扱うように情報を書き換え
            outSelf.self = this;
        }

        return hit;
    }

    /// <summary>
    /// デバッグ描画
    /// </summary>
    void DebugDraw() override
    {
        for (auto& c : colliders) c->DebugDraw(); // 各コライダーのデバッグ描画を呼び出す
    }

    // --- 変数定義 ---
    std::vector<std::unique_ptr<Collider>> colliders;           // コライダーのリスト
};
