#include "Hole.h"

// システム
#include "main.h"
#include "renderer.h"

// コンポーネント
#include "MeshRenderer.h"
#include "ColliderGroup.h"
#include "BoxCollider.h"

// ゲームオブジェクト
#include "Ball.h"

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - MeshRenderer を生成し、シェーダ/メッシュ/色を設定する
// - ColliderGroup + BoxCollider を生成し、当たり判定を構築する
void Hole::Init()
{
    GameObject::Init();

    // 表示用
    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);
    m_MeshRenderer->CreateUnitBox();
    m_MeshRenderer->m_Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f); // ダークグレー

    // 当たり判定用
    m_ColliderGroup = AddComponent<ColliderGroup>();
    (void)m_ColliderGroup->AddCollider<BoxCollider>(); // 現状は特別な設定は不要
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - 保持している非所有参照を解放（nullptr 化）する
void Hole::Uninit()
{
    m_MeshRenderer  = nullptr;
    m_ColliderGroup = nullptr;
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - 現状は親クラスの Update に委譲
void Hole::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 現状は親クラスの Draw に委譲
void Hole::Draw()
{
    GameObject::Draw();
}

// ------------------------------------------------------------------------------
// 衝突コールバック
// ------------------------------------------------------------------------------
// - Ball が衝突した場合はリセットする
void Hole::OnCollisionEnter(const CollisionInfo& info)
{
    GameObject* otherObj = info.other->m_Owner;

    // --------------------------------------------------------------------------
    // 1. ボールが落ちた場合
    // --------------------------------------------------------------------------
    if (auto* ball = dynamic_cast<Ball*>(otherObj))
    {
        ball->ResetBall();
        return;
    }

    // --------------------------------------------------------------------------
    // 2. エネミーがホールに入った場合（未実装）
    // --------------------------------------------------------------------------
    // TODO:
    // Enemyがホールに落ちた場合の処理を確認する
    // 別の部分で実装しているので、実装を一度確認すること
}