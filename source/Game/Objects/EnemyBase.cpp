#include "EnemyBase.h"
#include "Input.h"
#include "HP.h"
#include "EnemyManager.h"
#include "Score.h"

// ------------------------------------------------------------------------------
// システム / コンポーネント
// ------------------------------------------------------------------------------
#include "BoxCollider.h"
#include "ColliderGroup.h"
#include "AnimationModel.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"

// ------------------------------------------------------------------------------
// ゲームオブジェクト
// ------------------------------------------------------------------------------
#include "Ball.h"
#include "Hole.h"
#include "Camera.h"
#include "GameManager.h"

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - デフォルトのコンポーネント（現在は仮のモデルと BoxCollider）をセットアップ
// - EnemyManager への登録
void EnemyBase::Init()
{
    // 親クラスの初期化呼び出し
    GameObject::Init();

    // エネミーマネージャーへ登録
    EnemyManager::RegisterEnemy(this);
    // ------------------------------------------------------------------------------
    // パラメーター初期化
    // ------------------------------------------------------------------------------
    // Transformの初期設定
    // m_Transform.Scale = Vector3{ kDefaultEnemyScale, kDefaultEnemyScale, kDefaultEnemyScale };

    // モデル＆アニメーション関連は一旦オフにする
    /*
    // ------------------------------------------------------------------------------
    // AnimationModelコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_AnimationModel = AddComponent<AnimationModel>();
    m_AnimationModel->Load("asset\\model\\Akai.fbx");

    // TODO: 再生したいアニメーションがあればここで読み込み
    m_AnimationModel->LoadAnimation("asset\\model\\Akai_idle.fbx", "Idle");
    // m_AnimationModel->LoadAnimation("asset\\model\\enemy_run.fbx", "Run");

    // 初期状態はIdleをループ再生
    m_AnimationModel->Play("Idle", true);

    // ------------------------------------------------------------------------------
    // ColliderGroup + BoxColliderコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();
    BoxCollider* boxCollider = m_ColliderGroup->AddCollider<BoxCollider>();
    boxCollider->Center = Vector3{ 0.0f, 85.0f, 0.0f }; // モデルの中心に合わせて調整すること
    boxCollider->Size = Vector3{ 60.0f, 170.0f, 60.0f }; // モデルのサイズと合わないため、適宜調整
    */

    // ------------------------------------------------------------------------------
    // MeshRendererコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);
    m_MeshRenderer->CreateUnitBox(); // とりあえず箱メッシュで代用
    m_MeshRenderer->m_Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // 赤色に設定

    // ------------------------------------------------------------------------------
    // BoxColliderコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();
    BoxCollider* boxCollider = m_ColliderGroup->AddCollider<BoxCollider>();
    boxCollider->m_IsTrigger = true; // トリガーに設定
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - コンポーネントへの非所有ポインタのクリア
void EnemyBase::Uninit()
{
    // 親クラスの終了処理呼び出し
    GameObject::Uninit();

    // Componentsのポインタ解放
    // m_AnimationModel = nullptr;
    m_ColliderGroup = nullptr;
    m_MeshRenderer = nullptr;
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - Player(Target) 方向へ移動
// - アニメーションフレーム更新、死亡フラグ監視
void EnemyBase::Update(float deltaTime)
{
    // 親クラスの更新処理呼び出し
    GameObject::Update(deltaTime);

    // ターゲット方向へ移動する
    m_Transform.Position += GetDirToTarget() * (m_Speed * deltaTime);

    // アニメーションの更新
    // 状態に応じてアニメーションを切り替える場合はここで実装
    // _AnimationModel->Play("Run", true, false);
    ++m_AnimFrame;

    // HPが0以下ならIsDeadをtrueに設定
    if (m_HP <= 0)
    {
        m_IsDead = true;
    }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 基本は GameObject::Draw (コンポーネント描画) に委譲
void EnemyBase::Draw()
{
    // 親クラスの描画処理呼び出し
    GameObject::Draw();
}

// ------------------------------------------------------------------------------
// 衝突コールバック
// ------------------------------------------------------------------------------
// - ボールと衝突した際には ApplyDamage 経由でダメージ処理へ移行
// - ホールに入った際には、通常の OnKilled を介さず直接ペナルティ処理を行う
void EnemyBase::OnTriggerEnter(const CollisionInfo& info)
{
    GameObject* otherObj = info.other->m_Owner;

    // --- ボールと衝突したときの処理 ---
    if (auto* ball = dynamic_cast<Ball*>(otherObj))
    {
        // 外部メソッド経由でダメージを与える
        ApplyDamage(1);
    }

    // --- ホールに入ったときの処理 ---
    if (auto* hole = dynamic_cast<Hole*>(otherObj))
    {
        // ホールに入ったらペナルティ処理をして死亡（通常の撃破扱いではなく、ホールペナルティ）
        if (!m_IsDead)
        {
            m_IsDead = true;
            // ホールペナルティ処理を呼び出す
            HP::OnEnemyEnteredHole();
            Destroy();
        }
    }
}

// ------------------------------------------------------------------------------
// ダメージ処理
// ------------------------------------------------------------------------------
// - HPを減少し、0以下になったら OnKilled を呼び出す
void EnemyBase::ApplyDamage(int damage)
{
    if (m_IsDead) return;

    m_HP -= damage;
    if (m_HP <= 0)
    {
        OnKilled();
    }
}

// ------------------------------------------------------------------------------
// 即時撃破
// ------------------------------------------------------------------------------
// - ギミックによる強制死（ShockWaveなど）
void EnemyBase::Kill()
{
    if (m_IsDead) return;

    m_HP = 0;
    OnKilled();
}

// ------------------------------------------------------------------------------
// 死亡時の共通処理
// ------------------------------------------------------------------------------
// - HP回復・スコア等のシステム通知
// - カメラシェイク演出
// - 自信の破棄予約
void EnemyBase::OnKilled()
{
    if (m_IsDead) return; // 二重呼び出しガード

    m_IsDead = true;

    // エネミー撃破時のHP回復処理などを呼び出す
    HP::OnEnemyKilled();

    // 撃破スコアを加算する
    Score::AddScore(m_Score);

    // エネミー撃破時の小さなカメラシェイク (0.2秒、強さ1.0)
    for (auto obj : GameManager::GetGameObjects())
    {
        if (auto* cam = dynamic_cast<Camera*>(obj))
        {
            cam->StartShake(0.2f, 1.0f);
            break;
        }
    }

    // オブジェクト自身を破棄待ちキューに入れる
    Destroy();
}

// ------------------------------------------------------------------------------
// ターゲットへの正規化方向取得
// ------------------------------------------------------------------------------
Vector3 EnemyBase::GetDirToTarget() const
{
    Vector3 dir = m_TargetPos - m_Transform.Position;

    // ほぼゼロの場合は、(0,0,0)を返す
    if (dir.LengthSq() < 1e-6f)
    {
        return Vector3{ 0.0f, 0.0f, 0.0f };
    }
    // Vector3に備わっているNormalizeSafeを使用
    return dir.NormalizeSafe();
}