#include "EnemyBase.h"
#include "Input.h"

// コンポーネント
#include "SphereCollider.h"
#include "ColliderGroup.h"
#include "MeshRenderer.h"

// ゲームオブジェクト
#include "Ball.h"

// 初期化処理
void EnemyBase::Init()
{
    // 親クラスの初期化呼び出し
    GameObject::Init();

    // ------------------------------------------------------------------------------
    // MeshRendererコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);
    m_MeshRenderer->CreateUnitSphere();
    m_MeshRenderer->m_Color = XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f); // 赤色

    // ------------------------------------------------------------------------------
    // ColliderGroup + SphereColliderコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();
    SphereCollider* sphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();
    (void)sphereCollider; // 現状は特別な設定は不要
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