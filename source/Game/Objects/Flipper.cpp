#include "Flipper.h"
#include "Input.h"

// コンポーネント
#include "BoxCollider.h"
#include "ColliderGroup.h"
#include "MeshRenderer.h"
#include "RigidBody.h"

Flipper::Flipper(Side side)
    : m_Side(side)
{
}

// 初期化処理
void Flipper::Init()
{
    // 左右で基準角度を決める
    if (m_Side == Side::Left)
    {
        m_DefaultAngle = +30.0f;
        m_ActiveAngle  =   0.0f;
    }
    else // Right
    {
        m_DefaultAngle = -30.0f;
        m_ActiveAngle =    0.0f;
    }

    // 親オブジェクトは回転軸のみ
    m_Transform.Rotation.y = m_DefaultAngle;

    // ----------------------------------------------------------------------
    // メンバ変数初期化
    // ----------------------------------------------------------------------
    // 角度関連
    m_DefaultAngle = 0.0f;
    m_ActiveAngle  = 0.0f;
    m_AngularVelDegPerSec = 0.0f;

    // ----------------------------------------------------------------------
    // アーム用子オブジェクトの生成
    // ----------------------------------------------------------------------
    m_ArmObject = CreateChild();
    // 左右でアームの向きを変える
    if (m_Side == Side::Left)
    {
        // 左フリッパーは+X方向
        m_ArmObject->m_Transform.Position = { m_ArmLength * 0.5f, 0.0f, 0.0f }; // 回転軸からアーム中央まで移動
    }
    else
    {
        // 右フリッパーは-X方向
        m_ArmObject->m_Transform.Position = { -m_ArmLength * 0.5f, 0.0f, 0.0f }; // 回転軸からアーム中央まで移動
    }   
    m_ArmObject->m_Transform.Scale = { m_ArmLength, m_ArmHeight, m_ArmThickness };

    // ----------------------------------------------------------------------
    // メッシュレンダラー追加
    // ----------------------------------------------------------------------
    auto meshRenderer = m_ArmObject->AddComponent<MeshRenderer>();
    meshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);    // シェーダーの設定
    meshRenderer->CreateUnitBox();
    meshRenderer->m_Color = XMFLOAT4(0.9f, 0.9f, 0.95f, 1.0f);

    // ----------------------------------------------------------------------
    // ボックスコライダー追加
    // ----------------------------------------------------------------------
    auto colliderGroup = m_ArmObject->AddComponent<ColliderGroup>();
    colliderGroup->m_Owner = this;
    colliderGroup->m_Transform = &m_ArmObject->m_Transform;
    auto boxCollider = colliderGroup->AddCollider<BoxCollider>();
    (void)boxCollider; // 現在は特に設定なし
}

// 更新処理
void Flipper::Update(float deltaTime)
{
    // 更新前の角度を保存
    const float prevDeg = m_Transform.Rotation.y;

    // キー入力取得
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);

    // 目標角度（度数）
    float targetDeg = isPress ? m_ActiveAngle : m_DefaultAngle;

    // 現在角度（度数）
    float currentDeg = m_Transform.Rotation.y;

    // 1フレームで回せる最大角度（度）
    const float maxStep = kFlipperRotateSpeedDegPerSec * deltaTime;

    // 目標との差
    float diff = targetDeg - currentDeg;

    // approach: 1フレームに動かせる量をmaxstepに制限
    if (diff > maxStep)
    {
        currentDeg += maxStep;
    }
    else if (diff < -maxStep)
    {
        currentDeg -= maxStep;
    }
    else
    {
        currentDeg = targetDeg;
    }

    // 回転適用
    m_Transform.Rotation.y = currentDeg;

    // 角速度計算
    if (deltaTime > 0.0f)
    {
        m_AngularVelDegPerSec = (m_Transform.Rotation.y - prevDeg) / deltaTime;
    }
    else
    {
        m_AngularVelDegPerSec = 0.0f;
    }

    // 親のUpdateを呼ぶ
    GameObject::Update(deltaTime);
}

// 描画処理
void Flipper::Draw()
{
    GameObject::Draw();
}

// 終了処理
void Flipper::Uninit()
{
    m_ArmObject = nullptr;
}

// 動作キー取得
BYTE Flipper::GetActiveKey() const
{
    if (m_Side == Side::Left)
    {
        return VK_LSHIFT;
    }
    else // Right
    {
        return VK_OEM_2; // TODO: 後ほどバーチャルキーコードを定義する
    }
}

// 衝突コールバック
void Flipper::OnCollisionStay(const CollisionInfo& info)
{
    // 自分自身のコライダー以外は無視 
    if (info.self->m_Owner != this) return;

    // 情報チェック
    if (!info.self || !info.other) return;
    if (!info.self->m_Owner || !info.other->m_Owner) return;

    // 相手のRigidBody取得（ボール想定）
    RigidBody* rb = info.other->m_Owner->GetComponent<RigidBody>();
    if (!rb) return;

    // フリッパーを動かしているときだけ「弾く」
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);
    if (!isPress) return;

    // 最低限の角速度に達していないときは無視
    const float angVel = fabsf(m_AngularVelDegPerSec);
    if (angVel < kMinKickAngularVelDegPerSec) return;

    // --------------------------------------------------
    // 1) まずはめり込み解消（少しだけ押し戻す）
    // --------------------------------------------------

    // info.normal は「Box(フリッパー)を押し出す向き」＝球から見て「Sphere→Box」
    // ボールを外側に出したいので逆向き（Flipper→Ball）
    Vector3 n = -info.normal;

    // ここで「上方向成分」は捨てて、水平方向(XZ)だけを使う
    n.y = 0.0f;

    // 万が一、ほぼゼロになったら適当な横方向にする
    if (n.LengthSq() < 1e-6f)
    {
        // 左右で「外側」を変える
        float dirX = (m_Side == Side::Left) ? 1.0f : -1.0f;
        n = Vector3(dirX, 0.0f, 0.0f);
    }
    n = n.NormalizeSafe(); // 自前の安全Normalize

    const float kSeparateDist = 0.5f; // 押し戻す距離
    info.other->m_Owner->m_Transform.Position += n * kSeparateDist;

    // --------------------------------------------------
    // 2) 速度を「水平メイン＋ちょい上」に強制セット
    // -------------------------------------------------
    Vector3 newVel = n * kFlipperHorizontalSpeed;
    newVel.y = kFlipperUpSpeed;

    rb->m_Velocity = newVel;
}
