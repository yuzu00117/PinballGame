#include "EnemyBase.h"
#include "Input.h"

// コンポーネント
#include "SphereCollider.h"
#include "ColliderGroup.h"
#include "AnimationModel.h"

// ゲームオブジェクト
#include "Ball.h"

// 初期化処理
void EnemyBase::Init()
{
    // 親クラスの初期化呼び出し
    GameObject::Init();

    // ------------------------------------------------------------------------------
    // AnimationModelコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_AnimationModel = AddComponent<AnimationModel>();
    m_AnimationModel->Load("asset\\model\\Akai.fbx");
    m_AnimationModel->SetModelScale(kDefaultEnemyScale);

    // TODO: 再生したいアニメーションがあればここで読み込み
    m_AnimationModel->LoadAnimation("asset\\model\\Akai_idle.fbx", "Idle");
    // m_AnimationModel->LoadAnimation("asset\\model\\enemy_run.fbx", "Run");

    // 初期状態はIdleをループ再生
    m_AnimationModel->Play("Idle", true);

    // ------------------------------------------------------------------------------
    // ColliderGroup + SphereColliderコンポーネントの追加
    // ------------------------------------------------------------------------------
    m_ColliderGroup = AddComponent<ColliderGroup>();
    SphereCollider* sphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();
    (void)sphereCollider; // 現状は特別な設定は不要
}

// 終了処理
void EnemyBase::Uninit()
{
    // 親クラスの終了処理呼び出し
    GameObject::Uninit();

    // Componentsのポインタ解放
    m_AnimationModel = nullptr;
    m_ColliderGroup = nullptr;
}

// 更新処理
void EnemyBase::Update()
{
    // 親クラスの更新処理呼び出し
    GameObject::Update();

    // ターゲット方向へ移動する
    // m_Velocity = GetDirToTarget() * m_Speed;
    // m_Transform.Position += m_Velocity;

    // アニメーションの更新
    // 状態に応じてアニメーションを切り替える場合はここで実装
    // _AnimationModel->Play("Run", true, false);
    ++m_AnimFrame;
}

// 描画処理
void EnemyBase::Draw()
{
    // 親クラスの描画処理呼び出し
    GameObject::Draw();
}

// 衝突コールバック
void EnemyBase::OnCollisionEnter(const CollisionInfo& info)
{
    GameObject* otherObj = info.other->m_Owner;

    // ボールと衝突したときの処理
    if (auto* ball = dynamic_cast<Ball*>(otherObj))
    {
        // ここにボールと衝突したときの処理を記述
    }
}

// ターゲットへの正規化方向
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