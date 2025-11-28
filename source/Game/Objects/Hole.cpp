#include "main.h"
#include "renderer.h"
#include "Hole.h"

// コンポーネント関連
#include "MeshRenderer.h"
#include "ColliderGroup.h"
#include "BoxCollider.h"

// ゲームオブジェクト関連
#include "Ball.h"

// 初期化処理
void Hole::Init()
{
    // 親クラスの初期化
    GameObject::Init();

    // ------------------------------------------------------------------------------
    // メッシュレンダラーの追加・設定
    // ------------------------------------------------------------------------------
    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);
    m_MeshRenderer->CreateUnitBox();
    m_MeshRenderer->m_Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f); // ダークグレー

    // ------------------------------------------------------------------------------
    // 当たり判定用コライダーの追加・設定
    // ------------------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();
    auto box = m_ColliderGroup->AddCollider<BoxCollider>();
    (void)box; // 現状は特別な設定は不要
}

// 終了処理
void Hole::Uninit()
{
    // コンポーネントの解放
    m_MeshRenderer = nullptr;
    m_ColliderGroup = nullptr;
}

// 更新処理
void Hole::Update()
{
    // 親クラスの更新処理
    GameObject::Update();
}

// 描画処理
void Hole::Draw()
{
    // 親クラスの描画処理
    GameObject::Draw();
}

// 衝突コールバック
void Hole::OnCollisionEnter(const CollisionInfo& info)
{
    GameObject* otherObj = info.other->m_Owner;
    
    // ------------------------------------------------------------------------------
    // 1. ボールが落ちた場合
    // ------------------------------------------------------------------------------
    if (auto* ball = dynamic_cast<Ball*>(otherObj))
    {
        // ボールをリセット
        ball->ResetBall();
        return;
    }

    // -------------------------------------------------------------------------------
    // 2. エネミーがホールに入った場合
    // -------------------------------------------------------------------------------
    // まだ Enemy クラスが無いので、ここは TODO として残しておく。
    // Enemy クラスが GameObject から派生して定義できたら、
    // 例えば dynamic_cast<Enemy*>(otherObj) で判定して処理を書く想定。
    //
    // 例:
    //
    // if (auto* enemy = dynamic_cast<Enemy*>(otherObj))
    // {
    //     GameManager::Get()->AddHp(-1);
    //     GameManager::Get()->AddScore(-100);
    //     enemy->Destroy(); // 破棄
    // }

    // TODO: Enemy 実装後にここに処理を追加
}