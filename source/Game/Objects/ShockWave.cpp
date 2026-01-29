#include "ShockWave.h"


// コンポーネント
#include "ColliderGroup.h"
#include "SphereCollider.h"
#include "MeshRenderer.h"

// ゲームオブジェクト
#include "EnemyBase.h"
#include "HP.h"

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - ColliderGroup / SphereCollider を生成してトリガー判定を構築
// - SphereCollider 半径を開始値に設定
// - 経過時間をリセット
void ShockWave::Init()
{
    // ColliderGroup + SphereCollider を追加
    m_ColliderGroup = AddComponent<ColliderGroup>();
    m_SphereCollider = m_ColliderGroup->AddCollider<SphereCollider>();

    // Trigger 判定（物理反発なし）
    m_SphereCollider->m_IsTrigger = true;
    m_SphereCollider->m_radius = kStartRadius;

    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(kShockWaveVertexShaderPath, kShockWavePixelShaderPath);
    m_MeshRenderer->SetTexture(kShockWaveTexturePath);
    m_MeshRenderer->CreateUnitPlane();
    m_MeshRenderer->SetLocalScale(kStartRadius * 2.0f, 1.0f, kStartRadius * 2.0f);
    m_MeshRenderer->m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

    m_Elapsed = 0.0f;
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - 経過時間に応じて SphereCollider の半径を線形補間で拡大する
// - 寿命(kDuration)を超えたら Destroy() して削除予約する
// NOTE:
// - Destroy() は即時削除ではない（親の回収タイミングで破棄される）
void ShockWave::Update(float deltaTime)
{
    GameObject::Update(deltaTime);

    m_Elapsed += deltaTime;

    float t = (kDuration > 0.0f) ? (m_Elapsed / kDuration) : 1.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    const float radius = kStartRadius + (kEndRadius - kStartRadius) * t;

    // 非所有参照のため、生存確認してから操作する
    if (m_SphereCollider)
        m_SphereCollider->m_radius = radius;

    if (m_MeshRenderer)
    {
        m_MeshRenderer->SetLocalScale(radius * 2.0f, 1.0f, radius * 2.0f);
        const float alpha = 1.0f - t;
        m_MeshRenderer->m_Color = { 1.0f, 1.0f, 1.0f, alpha };
    }

    if (m_Elapsed >= kDuration)
    {
        Destroy();
    }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 現状は Component の Draw のみ（可視化は別途実装する）
// NOTE:
// - デバッグ描画（Collider DebugDraw）は GameObject::Draw() 側の設定に従う
void ShockWave::Draw()
{
    GameObject::Draw();
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - 非所有参照をクリア（安全のため）
// NOTE:
// - Component 実体の所有は GameObject 側（unique_ptr）
// - ShockWave 自身の破棄は Destroy() → 親の回収で行われる想定
void ShockWave::Uninit()
{
    m_ColliderGroup = nullptr;
    m_SphereCollider = nullptr;
    m_MeshRenderer = nullptr;
}

// ------------------------------------------------------------------------------
// トリガーコールバック
// ------------------------------------------------------------------------------
// - 接触相手の Owner が EnemyBase の場合、敵を Destroy() して撃破扱いにする
// - その際 HP::OnEnemyKilled() を呼び出し、撃破数などの進行を更新する
//
// 戻り値:
// - なし
//
// 副作用:
// - EnemyBase::Destroy() により対象敵が削除予約される
// - HP::OnEnemyKilled() によりゲーム進行状態が更新される
//
// NOTE:
// - 同一フレームで複数回入った場合、OnEnemyKilled() が複数回呼ばれ得る
//   （仕様として許容するか、Enemy側で一度だけ処理する設計にするか検討）
void ShockWave::OnTriggerEnter(const CollisionInfo& info)
{
    if (!info.other || !info.other->m_Owner) return;

    if (auto* enemy = dynamic_cast<EnemyBase*>(info.other->m_Owner))
    {
        enemy->Destroy();
        HP::OnEnemyKilled();
    }
}

