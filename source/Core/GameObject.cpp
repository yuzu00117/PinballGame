#include "gameobject.h"

// システム関連
#include <algorithm>

// コンポーネント関連
#include "ColliderGroup.h"
#include "Collider.h"

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - Component / 子オブジェクトを更新する
// - Update後に、Destroyフラグが立っている子オブジェクトを回収して破棄
void GameObject::Update(float deltaTime)
{
    // Component 更新
    for (auto& component : m_Components)
    {
        component->Update(deltaTime);
    }
    
    // 子オブジェクト 更新
    for (auto& child : m_Children)
    {
        child->Update(deltaTime);
    }

    // Destroyフラグが立っている子オブジェクトを回収して破棄（unique_ptrなので実体も破棄される）
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
// - Component / 子オブジェクトを描画する
// - デバッグ設定が有効な場合のみ、Collider を DebugDraw する
void GameObject::Draw()
{
    // Component 描画
    for (auto &component : m_Components)
    {
        component->Draw();
    }

    // 子オブジェクト 描画
    for (auto &child : m_Children)
    {
        child->Draw();
    }

    // デバッグ用：Collider 描画（有効な場合のみ）
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
// 子オブジェクトを追加する（基本版）
// ------------------------------------------------------------------------------
// 戻り値は非所有ポインタ（所有権は親GameObject側が保持：m_Childrenのunique_ptr）
// 生成直後に AttachChild により親子関係（m_Parent / Transformの親）が構築される
GameObject* GameObject::CreateChild()
{
    auto child = std::make_unique<GameObject>();
    GameObject *ptr = child.get(); // 非所有参照
    AttachChild(std::move(child)); // 所有権を親へ譲渡
    return ptr;
}

// ------------------------------------------------------------------------------
// 子オブジェクトをアタッチする
// ------------------------------------------------------------------------------
// child の所有権を受け取り、親子関係を設定する
// - m_Parent を設定
// - m_Transform の親を設定（親Transformに追従する）
void GameObject::AttachChild(std::unique_ptr<GameObject> child)
{
    child->m_Parent = this;
    child->m_Transform.SetParent(&m_Transform);
    m_Children.push_back(std::move(child));
}

// ------------------------------------------------------------------------------
// 子オブジェクトをデタッチする（親子関係も解除）
// ------------------------------------------------------------------------------
// NOTE: "Detach" という語感とは異なり、この実装は「所有を外へ渡す」のではなく
//       親子関係を解除したあとに、m_Children.clear() で全破棄している点に注意
void GameObject::DetachAllChildren()
{
    // 破棄前に親子関係（Transform / Parent参照）を解除しておく
    for (auto &child : m_Children)
    {
        child->m_Transform.ClearParent();
        child->m_Parent = nullptr;
    }
    // unique_ptr を破棄＝子オブジェクトもすべて破棄される
    m_Children.clear();
}

// ------------------------------------------------------------------------------
// 子オブジェクトも含めてすべての Collider を収集する
// ------------------------------------------------------------------------------
// - ColliderGroup は中に Collider を保持するため、グループ内をフラットに展開して追加する
// - それ以外の Collider はそのまま追加する
// - 子オブジェクトも再帰的に収集する
void GameObject::CollectCollidersRecursive(std::vector<Collider*>& outColliders)
{
    // 自分の Component から収集
    for (auto& component : m_Components)
    {
        // ColliderGroup は中身の Collider を展開して追加
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
        // それ以外の単体 Collider はそのまま追加
        else if (auto* collider = dynamic_cast<Collider*>(component.get()))
        {
            outColliders.push_back(collider);
        }
    }

    // 子オブジェクトも再帰的に収集
    for (auto& child : m_Children)
    {
        child->CollectCollidersRecursive(outColliders);
    }
}