#include "Bumper.h"

// コンポーネント
#include "SphereCollider.h"
#include "ColliderGroup.h"
#include "ModelRenderer.h"
#include "RigidBody.h"

#include <DirectXMath.h>

// ゲームオブジェクト
#include "Ball.h"
#include "ShockWave.h"

namespace
{
    // 衝撃波発生のクールダウン（秒）
    constexpr float kShockCooldown = 0.4f;
}

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - Transform の初期値を設定（Scale など）
// - ModelRenderer を追加し、モデルをロード
// - ColliderGroup + SphereCollider を追加し、判定半径を設定
void Bumper::Init()
{
    // ----------------------------------------------------------------------
    // オブジェクトのパラメータ設定
    // ----------------------------------------------------------------------
    m_Transform.Scale = { kBumperDefaultSize, kBumperDefaultSize, kBumperDefaultSize };

    // ----------------------------------------------------------------------
    // ModelRenderer コンポーネント追加
    // ----------------------------------------------------------------------
    m_ModelRenderer = AddComponent<ModelRenderer>();
    m_ModelRenderer->Load("asset//model//Bumper.obj");

    // ----------------------------------------------------------------------
    // SphereCollider コンポーネント追加
    // ----------------------------------------------------------------------
    auto* colliderGroup = AddComponent<ColliderGroup>();
    auto* sphereCollider = colliderGroup->AddCollider<SphereCollider>();
    sphereCollider->m_radius = kBumperDefaultColliderRadius;
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
void Bumper::Update(float deltaTime)
{
    GameObject::Update(deltaTime);

    if (m_ShockCooldownTimer > 0.0f)
    {
        m_ShockCooldownTimer -= deltaTime;
        if (m_ShockCooldownTimer < 0.0f)
            m_ShockCooldownTimer = 0.0f;
    }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 基本は Component 描画に委譲する
void Bumper::Draw()
{
    GameObject::Draw();
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - 本クラスは非所有参照のみを保持しているため nullptr へ戻す
// NOTE: Component の実体破棄は GameObject 側（unique_ptr）で行われる
void Bumper::Uninit()
{
    m_ModelRenderer = nullptr;
}

// ------------------------------------------------------------------------------
// 衝突コールバック
// ------------------------------------------------------------------------------
// - self/other の妥当性を検証
// - 相手が Ball で、RigidBody を持つ場合のみキックを適用
// - めり込み対策として、法線方向へ少し押し出してから速度を上書きする
// NOTE:
// - 速度を「上書き」しているため、既存速度は消える（意図が違う場合は加算に変更する）
// - info.normal の向きは self->other / other->self に依存するため、符号は仕様として固定すること
void Bumper::OnCollisionEnter(const CollisionInfo& info)
{
    // ----------------------------------------------------------------------
    // 早期リターン：入力の妥当性チェック
    // ----------------------------------------------------------------------
    if (!info.self || !info.other) return;
    if (info.self->m_Owner != this) return;
    if (!info.other->m_Owner) return;

    // ----------------------------------------------------------------------
    // 衝突相手がボールか確認
    // ----------------------------------------------------------------------
    auto* ball = dynamic_cast<Ball*>(info.other->m_Owner);
    if (!ball) return;

    // ----------------------------------------------------------------------
    // ボールの RigidBody を取得
    // ----------------------------------------------------------------------
    auto* rb = ball->GetComponent<RigidBody>();
    if (!rb) return;

    // ----------------------------------------------------------------------
    // 衝撃波の生成：クールダウン中は生成しない
    // ----------------------------------------------------------------------
    if (m_ShockCooldownTimer <= 0.0f)
    {
        auto* shockWave = CreateChild<ShockWave>();
        shockWave->Init();

        // 子オブジェクトなのでローカル原点に置く（親Transformでワールド位置が決まる）
        shockWave->m_Transform.Position = Vector3{ 0.0f, 0.0f, 0.0f };

        m_ShockCooldownTimer = kShockCooldown;
    }

    // ----------------------------------------------------------------------
    // キック方向を計算（水平成分のみ法線から作り、Y は固定で上方向）
    // ----------------------------------------------------------------------
    Vector3 n = -info.normal; // NOTE: 衝突法線の定義に合わせて符号を固定
    n.y = 0.0f;

    // 方向がゼロに近い場合はフォールバック
    if (n.LengthSq() < 1e-6f)
        n = Vector3(1.0f, 0.0f, 0.0f);

    n = n.NormalizeSafe();

    // ----------------------------------------------------------------------
    // めり込み対策：少し押し出す
    // ----------------------------------------------------------------------
    constexpr float kSeparateDist = 0.3f;
    ball->m_Transform.Position += n * kSeparateDist;

    // ----------------------------------------------------------------------
    // 速度を設定してキック（上書き）
    // ----------------------------------------------------------------------
    Vector3 newVel = n * kBumperKickHorizontalSpeed;
    newVel.y = kBumperKickVerticalSpeed;
    rb->m_Velocity = newVel;
}
