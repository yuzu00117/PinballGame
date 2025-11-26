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

// 更新処理
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

void Flipper::OnCollisionEnter(const CollisionInfo& info)
{
    // フリッパーのアームと当たったときだけ
    if (!info.self || !info.other) return;
    if (info.self->m_Owner != m_ArmObject) return;

    RigidBody* rb = info.other->m_Owner->GetComponent<RigidBody>();
    if (!rb) return;

    // フリッパーが動いてないなら何もしない
    const BYTE key = GetActiveKey();
    if (!Input::GetKeyPress(key)) return;

    // 反射方向（Flipper → Ball）
    Vector3 n = -info.normal;
    // n = Normalize(n); // 必要なら正規化

    // --------------------------
    // 1) ボール位置を確実に外に押し出す
    // --------------------------
    const float separate = 0.3f;
    info.other->m_Owner->m_Transform.Position += n * separate;

    // --------------------------
    // 2) ボール速度を強制的に反射させる
    // --------------------------

    // v' = v - 2*(v·n)*n の完全鏡面反射
    float vn = rb->m_Velocity.Dot(n);
    Vector3 reflected = rb->m_Velocity - n * (2.0f * vn);

    // --------------------------
    // 3) 最低速度を与えてパンチ力UP
    // --------------------------
    const float minSpeed = 350.0f;  // 好みで調整（200〜300でもOK）
    float speed = reflected.Length();

    if (speed < minSpeed)
    {
        reflected = reflected.NormalizeSafe() * minSpeed;
    }

    rb->m_Velocity = reflected;
}

// 衝突コールバック
void Flipper::OnCollisionStay(const CollisionInfo& info)
{
    // 情報チェック
    if (!info.self || !info.other) return;
    if (!info.self->m_Owner || !info.other->m_Owner) return;

    // 「アーム」についているコライダーとの衝突のみ処理
    if (info.self->m_Owner != m_ArmObject) return;

    // 相手のRigidBody取得（ボール想定）
    RigidBody* rb = info.other->m_Owner->GetComponent<RigidBody>();
    if (!rb) return;

    // フリッパーを動かしているときだけ「弾く」
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);
    if (!isPress) return;

    // ------------------------------------------------------------------
    // 1) 位置の押し戻し
    // ------------------------------------------------------------------

    // info.normalは「Box(フリッパー)を押し出す向き」なので、
    // ボールを押し出したいときは逆向き（Flipper -> Ball）
    Vector3 n = -info.normal;

    // 正規化できるなら正規化（Vector3のNormalizeメソッド/関数に合わせて書き換えてください）
    // n.Normalize();
    // or
    // n = Normalize(n);

    // 少しだけ確実に外側に出す
    const float kSeparateDist = 0.2f;   // テーブルスケールに合わせて調整
    info.other->m_Owner->m_Transform.Position += n * kSeparateDist;

    // ------------------------------------------------------------------
    // 2) 速度の設定（「最低限これだけは出す」）
    // ------------------------------------------------------------------

    // テーブルのスケールに合わせて調整する値
    const float kBaseSpeed = 180.0f;     // 最低限、このくらいの速さで飛ばす
    const float kExtraBoostMax = 120.0f; // もともと速度があったときの上乗せ上限

    // 法線方向と接線方向に分解
    float vN = rb->m_Velocity.Dot(n);    // 法線成分の大きさ
    Vector3 vNVec = n * vN;              // 法線成分ベクトル
    Vector3 vT = rb->m_Velocity - vNVec; // 接線成分

    // 接線成分は少しだけ残す（0.0〜1.0）
    const float kTangentialKeepRate = 0.4f; // 0に近いほど「フリッパー方向にガッツリ」

    vT *= kTangentialKeepRate;

    // 常に「外向き」に十分な速度を持たせる
    float newVN = kBaseSpeed;

    if (vN > 0.0f)
    {
        // すでに外向き成分を持っていたら、そこに少し上乗せ
        newVN = vN + kExtraBoostMax;
        if (newVN < kBaseSpeed)
        {
            newVN = kBaseSpeed;
        }
    }

    // 最終速度：接線は少しだけ残しつつ、法線成分を強制設定
    rb->m_Velocity = vT + n * newVN;
}
