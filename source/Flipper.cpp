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
    auto boxCollider = colliderGroup->AddCollider<BoxCollider>();
    (void)boxCollider; // 現在は特に設定なし
}

void Flipper::Update()
{
    // キー入力取得
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);

    // 目標角度（度数）
    float targetDeg = isPress ? m_ActiveAngle : m_DefaultAngle;

    // 現在角度（度数）
    float currentDeg = m_Transform.Rotation.y;

    // 1フレームで回せる最大角度（度）
    const float maxStep = kFlipperRotateSpeedDegPerSec * kDeltaTime;

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

    // 今回は一気に切り替え（必要なら補間処理を追加）
    m_Transform.Rotation.y = currentDeg;

    // 親のUpdateを呼ぶ
    GameObject::Update();
}

void Flipper::Draw()
{
    GameObject::Draw();
}

void Flipper::Uninit()
{
    m_ArmObject = nullptr;
}

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

void Flipper::OnCollisionStay(const CollisionInfo& info)
{
    // 自分または相手の情報がない場合は処理しない
    if (!info.self || !info.other) return;
    if (!info.self->m_Owner || !info.other->m_Owner) return;

    // フリッパー本体ではなく、「アーム」についているコライダーとの衝突のみ処理
    // （Fieldなど他コライダーなどで誤動作しないようにするため）
    if (info.self->m_Owner != m_ArmObject) return;

    // 衝突相手のRigidBodyを取得
    RigidBody* otherRigidBody = info.other->m_Owner->GetComponent<RigidBody>();
    if (!otherRigidBody) return;

    // フリッパーが動いていないときは「普通の壁」として振る舞う
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);
    if (!isPress) return;

    // TODO: 将来的にはエンジン側で「回転しているコライダーの相対速度」や
    //       角速度を考慮した接触解決を実装し、ここではその結果だけを受け取るようにする

    // info.normalは「Boxを押し出す向き」なので、
    // ボールを弾きたい場合は逆向きにする（flipper→ballの向き）
    Vector3 n = -info.normal;

    // 当面の簡易実装
    // 衝突法線方向に一定の速度を上乗せして「弾いている感じ」を出す
    const float kAddSpeed = 30.0f; // 上乗せ速度

    otherRigidBody->m_Velocity += n * kAddSpeed;
}