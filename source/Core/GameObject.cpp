#include "gameobject.h"
#include "ColliderGroup.h"
#include "Collider.h"

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
void GameObject::Update()
{
    // コンポーネントの更新
    for (auto& Component : m_Components) Component->Update();
    // 子オブジェクトの更新
    for (auto& Child : m_Children) Child->Update();

    // Destroyフラグが立っている子オブジェクトを削除
    m_Children.erase(
        std::remove_if(
            m_Children.begin(),
            m_Children.end(),
            [](const std::unique_ptr<GameObject>& child) {
                return child->IsDead();
            }),
        m_Children.end());
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
void GameObject::Draw()
{
    // コンポーネントの描画
    for (auto &Component : m_Components)
        Component->Draw();
    // 子オブジェクトの描画
    for (auto &Child : m_Children)
        Child->Draw();

    // デバッグ用コライダー描画
    if (g_EnableColliderDebugDraw)
    {
        for (auto &component : m_Components)
        {
            if (auto collider = dynamic_cast<Collider *>(component.get()))
            {
                collider->DebugDraw();
            }
        }
    }
}

// ------------------------------------------------------------------------------
// 子オブジェクトを追加する
// ------------------------------------------------------------------------------
GameObject* GameObject::CreateChild()
{
    auto child = std::make_unique<GameObject>();
    GameObject *ptr = child.get();
    AttachChild(std::move(child));
    return ptr;
}

// ------------------------------------------------------------------------------
// 子オブジェクトをアタッチする
// ------------------------------------------------------------------------------
void GameObject::AttachChild(std::unique_ptr<GameObject> child)
{
    child->m_Parent = this;
    child->m_Transform.SetParent(&m_Transform);
    m_Children.push_back(std::move(child));
}

// ------------------------------------------------------------------------------
// 子オブジェクトをデタッチする
// ------------------------------------------------------------------------------
void GameObject::DetachAllChildren()
{
    for (auto &Child : m_Children)
    {
        Child->m_Transform.ClearParent();
        Child->m_Parent = nullptr;
    }
    m_Children.clear();
}

// ------------------------------------------------------------------------------
// 再帰的に自分と子オブジェクトのコライダーを取得
// ------------------------------------------------------------------------------
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