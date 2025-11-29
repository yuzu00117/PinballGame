#include "EnemyBase.h"
#include "Input.h"

// コンポーネント
#include "BoxCollider.h"
#include "ColliderGroup.h"
#include "MeshRenderer.h"

// 初期化処理
void EnemyBase::Init()
{
}

// 更新処理
void EnemyBase::Update()
{
    GameObject::Update();
}

// 描画処理
void EnemyBase::Draw()
{
    GameObject::Draw();
}

// 終了処理
void EnemyBase::Uninit()
{
}

// 衝突コールバック
void EnemyBase::OnCollisionEnter(const CollisionInfo& info)
{
}