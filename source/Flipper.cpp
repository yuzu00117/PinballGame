#include "main.h"
#include "renderer.h"
#include "flipper.h"
#include "modelRenderer.h"
#include "input.h"

static inline float Clamp(float v, float a, float b){ return std::max(a, std::min(b, v)); }
static inline float DotXZ(const Vector3& a, const Vector3& b){ return a.x*b.x + a.z*b.z; }
static inline float LenXZ(const Vector3& v){ return std::sqrt(std::max(0.0f, DotXZ(v,v))); }
static inline Vector3 NormalizeXZ(const Vector3& v){
    float l = LenXZ(v);
    return (l>1e-6f) ? Vector3{v.x/l, 0.0f, v.z/l} : Vector3{0,0,0};
}
// 2D( XZ )の 90°回転ベクトル
static inline Vector3 PerpXZ(const Vector3& v){ return Vector3{-v.z, 0.0f, v.x}; }
// +X を基準に角度回転した単位ベクトル（XZ）
static inline Vector3 DirFromAngle(float rad){
    float c = std::cos(rad), s = std::sin(rad);
    return Vector3{ c, 0.0f, s };
}

// 初期化処理
void Flipper::Init()
{
    // シェーダや頂点バッファは必要になった時に用意してください。
    // ここでは最小限：描画は ModelRenderer が存在する時のみ行います。
    m_PrevAngle = m_CurrentAngle;

    // モデル読み込み
    m_ModelRenderer = new ModelRenderer();
    m_ModelRenderer->Load("asset\\model\\FlipperTest.obj");

    // シェーダー作成
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

// 終了処理
void Flipper::Uninit()
{
    // モデルレンダラーの解放
    delete m_ModelRenderer;
    m_ModelRenderer = nullptr;

    // シェーダーの解放
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();
}

// 更新処理
void Flipper::Update()
{
    const float dt = 1.0f/60.0f; // 固定更新の想定（可変なら差し替え）

    m_PrevAngle = m_CurrentAngle;

    const bool pressed = Input::GetKeyPress(m_Desc.key);
    const float target = pressed ? m_Desc.maxAngle : m_Desc.restAngle;
    const float speed  = pressed ? m_Desc.upSpeed   : m_Desc.downSpeed;

    const float delta = target - m_CurrentAngle;
    const float step  = Clamp(delta, -speed*dt, speed*dt);
    m_CurrentAngle += step;
}

// 描画処理
void Flipper::Draw()
{
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);

    Vector3 A, B; GetSegment(A, B);
    Vector3 mid = (A + B) * 0.5f;
    Vector3 dir = Vector3{ B.x - A.x, 0.0f, B.z - A.z };
    const float len = LenXZ(dir);
    const float yaw = std::atan2(dir.z, dir.x);

    // モデルは「長さ1の棒」を想定してスケーリング（任意）
    XMMATRIX S = XMMatrixScaling(len, m_Desc.thickness, m_Desc.thickness);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
    XMMATRIX T = XMMatrixTranslation(mid.x, m_Desc.pivot.y, mid.z);
    XMMATRIX W = S * R * T;

    Renderer::SetWorldMatrix(W);
    m_ModelRenderer->Draw();
}

// フリッパーの先端と根元の位置を取得
void Flipper::GetSegment(Vector3& outA, Vector3& outB) const
{
    outA = m_Desc.pivot;
    const float sign = (m_Desc.invert ? -1.0f : 1.0f);
    const Vector3 dir = DirFromAngle(m_CurrentAngle * sign);
    outB = outA + dir * m_Desc.length;
}

// ボールとの衝突
void Flipper::Resolve(Vector3& ballPosition, Vector3& ballVelocity, float ballRadius, float DeltaTime)
{
    // フリッパーは XZ の線分＋カプセル半径、ボールは球だが XZ では円扱い
    Vector3 A, B; GetSegment(A, B);
    const float rCaps = m_Desc.thickness * 0.5f;
    const float R = rCaps + ballRadius;

    // 高さ帯が大きくズレているなら衝突しない
    if (std::fabs(ballPosition.y - m_Desc.pivot.y) > (ballRadius + rCaps + 0.2f))
        return;

    const Vector3 AB = B - A;
    const float ab2  = std::max(1e-6f, DotXZ(AB, AB));
    const Vector3 AP = ballPosition - A;
    const float t    = Clamp(DotXZ(AP, AB) / ab2, 0.0f, 1.0f);
    const Vector3 closest = A + AB * t;

    Vector3 diff{ ballPosition.x - closest.x, 0.0f, ballPosition.z - closest.z };
    const float dist = LenXZ(diff);

    if (dist < R)
    {
        const float pen = R - dist;
        Vector3 n = (dist > 1e-6f) ? NormalizeXZ(diff) : NormalizeXZ(PerpXZ(AB));

        // 1) 位置補正（XZ 平面のみ）
        ballPosition.x += n.x * pen;
        ballPosition.z += n.z * pen;

        // 2) 速度反射＋先端速度ブースト
        float sign   = (m_Desc.invert ? -1.0f : 1.0f);
        float angVel = (m_CurrentAngle - m_PrevAngle) / std::max(1e-6f, DeltaTime);
        angVel *= sign;

        // ピボット基準の最近点ベクトル r
        Vector3 r{ closest.x - m_Desc.pivot.x, 0.0f, closest.z - m_Desc.pivot.z };
        // v_flip = ω × r（2D換算で r を 90°回転させて ω を掛ける）
        Vector3 vFlip = PerpXZ(r);
        vFlip.x *= angVel; vFlip.z *= angVel;

        Vector3 vBallXZ{ ballVelocity.x, 0.0f, ballVelocity.z };
        Vector3 vRel{ vBallXZ.x - vFlip.x, 0.0f, vBallXZ.z - vFlip.z };

        const float vn = vRel.x*n.x + vRel.z*n.z; // 法線方向の相対速度
        if (vn < 0.0f)
        {
            const float e = m_Desc.restitution;

            // 有効質量：フリッパーは固定とみなしボールのみ（=1）
            const float invMass = 1.0f;

            // 法線インパルス
            const float j = -(1.0f + e) * vn / invMass;

            // フリッパー先端速度の法線成分をブースト
            const float vFlipN = std::max(0.0f, vFlip.x*n.x + vFlip.z*n.z);
            const float impulse = j + (m_Desc.hitBoost * vFlipN);

            ballVelocity.x += n.x * impulse * invMass;
            ballVelocity.z += n.z * impulse * invMass;
            // y は床や重力の既存ロジックに任せる
        }
    }
}