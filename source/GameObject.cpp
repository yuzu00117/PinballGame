#include "gameobject.h"
#include "ColliderGroup.h"
#include "Collider.h"

// 再帰的に自分と子オブジェクトンのコライダーを取得
void GameObject::CollectCollidersRecursive(std::vector<Collider*>& outColliders)
{
    // 自分のコンポーネント
    for (auto& component : m_Components)
    {
        // ColliderGroupの場合は中身をフラットに追加
        if (auto* colliderGroup = dynamic_cast<ColliderGroup*>(component.get()))
        {
            for (auto& collider : colliderGroup->colliders)
            {
                if (collider)
                {
                    outColliders.push_back(collider.get());
                }
            }
        }
        // それ以外のColliderの場合はそのまま追加
        else if (auto* collider = dynamic_cast<Collider*>(component.get()))
        {
            outColliders.push_back(collider);
        }
    }

    // 子オブジェクトのコライダーも再帰的に取得
    for (auto& child : m_Children)
    {
        child->CollectCollidersRecursive(outColliders);
    }
}