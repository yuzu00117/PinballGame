#include "Bumper.h"
#include "Input.h"

// コンポーネント
#include "SphereCollider.h"
#include "ColliderGroup.h"
#include "ModelRenderer.h"
#include "RigidBody.h"

// 初期化処理
void Bumper::Init()
{
    // ----------------------------------------------------------------------
    // ModelRendererコンポーネント追加
    // ----------------------------------------------------------------------
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//BumperTest.obj");

    // ----------------------------------------------------------------------
    // SphereColliderコンポーネント追加
    // ----------------------------------------------------------------------
    auto colliderGroup = AddComponent<ColliderGroup>();
    SphereCollider* sphereCollider = colliderGroup->AddCollider<SphereCollider>();
    sphereCollider->m_radius = kDefaultColliderRadius; // 半径を設定
}

// 更新処理
void Bumper::Update(float deltaTime)
{

}

// 描画処理
void Bumper::Draw()
{
    GameObject::Draw();
}

// 終了処理
void Bumper::Uninit()
{
    m_ModelRenderer = nullptr;
}

// 衝突コールバック
void Bumper::OnCollisionStay(const CollisionInfo& info)
{
}
