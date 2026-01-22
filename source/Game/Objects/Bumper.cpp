#include "Bumper.h"

// システム関連
#include "Input.h"

// コンポーネント
#include "SphereCollider.h"
#include "ColliderGroup.h"
#include "ModelRenderer.h"
#include "RigidBody.h"
#include "Ball.h"

// 初期化処理
void Bumper::Init()
{
    // ----------------------------------------------------------------------
    // オブジェクトのパラメータ設定
    // ----------------------------------------------------------------------
    m_Transform.Scale = { kBumperDefaultSize, kBumperDefaultSize, kBumperDefaultSize }; // スケール設定

    // ----------------------------------------------------------------------
    // ModelRendererコンポーネント追加
    // ----------------------------------------------------------------------
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//Bumper.obj");

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
void Bumper::OnCollisionEnter(const CollisionInfo& info)
{
    // ----------------------------------------------------------------------
    // ボールを弾く処理
    // ----------------------------------------------------------------------
    // 早期リターン
    if (!info.self || !info.other) return;  // self, otherが何もない
    if (info.self->m_Owner != this) return; // selfが自分自身でない
    if (!info.other->m_Owner) return;       // otherの所有者がいない

    // 衝突相手がボールか確認
    auto* ball = dynamic_cast<Ball*>(info.other->m_Owner);
    if (!ball) return;

    // ボールのRigidBodyを取得
    RigidBody* rb = ball->GetComponent<RigidBody>();
    if (!rb) return;

    // 衝突方向から、ボールを弾く速度ベクトルを計算して設定
    Vector3 n = -info.normal;
    n.y = 0.0f;

    // 方向がゼロベクトルの場合の対策
    if (n.LengthSq() < 1e-6f)
    {
        n = Vector3(1.0f, 0.0f, 0.0f);
    }
    n = n.NormalizeSafe();

    // ボールを少し押し出す（めり込み対策）
    const float kSeparateDist = 0.3f;
    ball->m_Transform.Position += n * kSeparateDist;

    // 弾く速度を設定して弾く
    Vector3 newVel = n * kBumperKickHorizontalSpeed;
    newVel.y = kBumperKickVerticalSpeed;
    rb->m_Velocity = newVel;
}