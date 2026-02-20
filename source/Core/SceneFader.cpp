#include "SceneFader.h"

#include "Sprite.h"
#include "Easing.h"
#include "MathUtil.h"

// ------------------------------------------------------------------------------
// 静的メンバ変数の定義
// ------------------------------------------------------------------------------
SceneFader::State                 SceneFader::m_State      = SceneFader::State::Idle;
float                             SceneFader::m_Timer      = 0.0f;
float                             SceneFader::m_Duration   = 0.5f;
float                             SceneFader::m_Alpha      = 0.0f;
std::function<void()>             SceneFader::m_OnMidpoint = nullptr;
Sprite*                           SceneFader::m_Overlay    = nullptr;

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
// - 黒オーバーレイ用スプライトを生成し、全画面サイズに設定する
// NOTE: Renderer 初期化後に呼ぶこと
void SceneFader::Init()
{
    m_Overlay = new Sprite();
    m_Overlay->Init();
    m_Overlay->SetColor(0.0f, 0.0f, 0.0f, 0.0f); // 初期は完全透明
    m_Overlay->SetPosition(0.0f, 0.0f);
    m_Overlay->SetSize(
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT));
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
// - m_Overlay は GameObject の子ではないため、ここで明示的に解放する
void SceneFader::Uninit()
{
    if (m_Overlay)
    {
        m_Overlay->Uninit();
        delete m_Overlay;
        m_Overlay = nullptr;
    }
}

// ------------------------------------------------------------------------------
// フェード開始
// ------------------------------------------------------------------------------
// - 既にフェード中の場合は何もしない（二重呼び出し防止）
// - State を FadeOut に設定し、タイマーをリセットする
void SceneFader::BeginFade(std::function<void()> onMidpoint, float duration)
{
    if (m_State != State::Idle) return;

    m_OnMidpoint = onMidpoint;
    m_Duration   = duration;
    m_Timer      = 0.0f;
    m_Alpha      = 0.0f;
    m_State      = State::FadeOut;
}

// ------------------------------------------------------------------------------
// 毎フレーム更新処理
// ------------------------------------------------------------------------------
// - FadeOut : alpha を 0→1 に EaseInQuad で変化させる
//             完了時にコールバックを実行し、FadeIn へ遷移する
// - FadeIn  : alpha を 1→0 に EaseOutQuad で変化させる
//             完了時に Idle へ戻る
void SceneFader::Update(float deltaTime)
{
    if (m_State == State::Idle) return;

    m_Timer += deltaTime;
    const float t = Clamp(m_Timer / m_Duration, 0.0f, 1.0f);

    if (m_State == State::FadeOut)
    {
        m_Alpha = EaseInQuad(t); // 0 → 1（加速しながら暗くなる）

        if (m_Timer >= m_Duration)
        {
            m_Alpha = 1.0f;

            // フェードアウト完了 → シーン切替コールバックを実行
            if (m_OnMidpoint)
            {
                m_OnMidpoint();
                m_OnMidpoint = nullptr;
            }

            // フェードインへ遷移
            m_Timer = 0.0f;
            m_State = State::FadeIn;
        }
    }
    else if (m_State == State::FadeIn)
    {
        m_Alpha = 1.0f - EaseOutQuad(t); // 1 → 0（減速しながら明るくなる）

        if (m_Timer >= m_Duration)
        {
            m_Alpha = 0.0f;
            m_State = State::Idle;
        }
    }

    // アルファ値をオーバーレイスプライトに反映
    if (m_Overlay)
    {
        m_Overlay->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
    }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - アルファが 0 の場合は描画をスキップする（Idle 時の無駄な描画を防ぐ）
// NOTE: 全シーン描画の最後に呼ぶこと
void SceneFader::Draw()
{
    if (m_State == State::Idle) return;
    if (!m_Overlay) return;

    m_Overlay->Draw();
}

// ------------------------------------------------------------------------------
// フェード中かどうかの判定
// ------------------------------------------------------------------------------
bool SceneFader::IsFading()
{
    return m_State != State::Idle;
}
