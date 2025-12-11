#include "EnemyBase.h"
#include "Input.h"

// コンポーネント
#include "BoxCollider.h"
#include "ColliderGroup.h"
#include "AnimationModel.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"

// ゲームオブジェクト
#include "Ball.h"

// 初期化処理
void EnemyBase::Init()
{
    // 親クラスの初期化呼び出し
    GameObject::Init();
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
    (void)boxCollider; // 設定無し

    // ----------------------------------------------------------------------
    // Rigidbodyコンポーネントの追加
    // ----------------------------------------------------------------------
    m_RigidBody = AddComponent<RigidBody>();
    m_RigidBody->m_Restitution = 0.0f;         // 反発係数を設定
    m_RigidBody->m_UseGravity = true;          // 重力を有効化
    m_RigidBody->m_IsKinematic = false;        // キネマティック無効化
}

// 終了処理
void EnemyBase::Uninit()
{
    // 親クラスの終了処理呼び出し
    GameObject::Uninit();

    // Componentsのポインタ解放
    // m_AnimationModel = nullptr;
    m_ColliderGroup = nullptr;
    m_MeshRenderer = nullptr;
}

// 更新処理
void EnemyBase::Update()
{
    // 親クラスの更新処理呼び出し
    GameObject::Update();

    // ターゲット方向へ移動する
    m_Velocity = GetDirToTarget() * m_Speed;
    m_Transform.Position += m_Velocity;

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
        m_HP -= 1; // ボールと衝突したらHPを1減らす
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