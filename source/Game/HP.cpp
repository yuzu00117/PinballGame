#include "HP.h"
#include "Renderer.h"
#include "Input.h"
#include <algorithm>
#include <string>
#include "MathUtil.h"
#include "../Graphics/Sprite.h"

// ------------------------------------------------------------------------------
// static メンバ変数定義
// - HP 状態はゲーム全体で単一。Init でデフォルト値に上書きされる
// ------------------------------------------------------------------------------
float HP::s_HP          = 0.0f;
float HP::s_MaxHP       = 0.0f;
float HP::s_DrainPerSec = 0.0f;
float HP::s_HolePenalty = 0.0f;
float HP::s_KillHeal    = 0.0f;

// ------------------------------------------------------------------------------
// Init
// - HP・パラメータをデフォルト値で初期化する
// - HPボードスプライトと HPバースプライトを生成・配置する
// NOTE: テクスチャなしの m_BarSprite は SetColor のみで色を表現する
// ------------------------------------------------------------------------------
void HP::Init()
{
    s_MaxHP       = kDefaultMaxHP;
    s_HP          = s_MaxHP;
    s_DrainPerSec = kDefaultDrainPerSec;
    s_HolePenalty = kDefaultHolePenalty;
    s_KillHeal    = kDefaultKillHeal;

    m_BoardSprite = std::make_unique<Sprite>();
    m_BoardSprite->Init();
    m_BoardSprite->SetTexture(L"asset/texture/HPBoard.png");
    m_BoardSprite->SetSize(kBoardWidth, kBoardHeight);
    m_BoardSprite->SetPosition(kBoardX, kBoardY);

    m_BarSprite = std::make_unique<Sprite>();
    m_BarSprite->Init();
    m_BarSprite->SetSize(kBarMaxWidth, kBarHeight);
    m_BarSprite->SetPosition(kBoardX + kBarOffsetX, kBoardY + kBarOffsetY);
    m_BarSprite->SetColor(0.2f, 0.9f, 0.2f);
}

// ------------------------------------------------------------------------------
// Uninit
// - 所有スプライトを Uninit してから解放する
// ------------------------------------------------------------------------------
void HP::Uninit()
{
    if (m_BoardSprite)
    {
        m_BoardSprite->Uninit();
        m_BoardSprite.reset();
    }

    if (m_BarSprite)
    {
        m_BarSprite->Uninit();
        m_BarSprite.reset();
    }
}

// ------------------------------------------------------------------------------
// Update
// - 毎フレーム HP Drain を実行する
// - HP 比率に応じてバーの幅と色を更新する（緑 / 黄 / 赤の3段階）
// ------------------------------------------------------------------------------
void HP::Update(float deltaTime)
{
    Drain(deltaTime);

    if (m_BarSprite)
    {
        const float ratio    = GetHP01();
        const float barWidth = kBarMaxWidth * ratio;
        m_BarSprite->SetSize(barWidth, kBarHeight);

        if      (ratio >= 0.6f) { m_BarSprite->SetColor(0.2f,  0.9f,  0.2f); }  // 緑
        else if (ratio >= 0.3f) { m_BarSprite->SetColor(0.95f, 0.85f, 0.1f); }  // 黄
        else                    { m_BarSprite->SetColor(0.9f,  0.2f,  0.2f); }  // 赤
    }

#if defined(_DEBUG)
    // ------------------------------------------------------------------------------
    // デバッグ操作
    // - H: 回復 +10 / J: ダメージ -10
    // - K: エネミーがホールに入った / L: エネミー撃破
    // ------------------------------------------------------------------------------
    if (Input::GetKeyTrigger('H')) { AddHP(10.0f); }
    if (Input::GetKeyTrigger('J')) { AddHP(-10.0f); }
    if (Input::GetKeyTrigger('K')) { OnEnemyEnteredHole(); }
    if (Input::GetKeyTrigger('L')) { OnEnemyKilled(); }
#endif
}

// ------------------------------------------------------------------------------
// Draw
// - HPボード画像を描画する
// - HPバースプライトを描画する
// ------------------------------------------------------------------------------
void HP::Draw()
{
    if (m_BoardSprite) { m_BoardSprite->Draw(); }
    if (m_BarSprite)   { m_BarSprite->Draw(); }

    // TODO: HPテキスト数値の描画（Renderer::DrawText 実装後に対応）
    // int hp = static_cast<int>(s_HP);
    // std::wstring text = std::to_wstring(hp);
    // const float textX = kBoardX + kBarOffsetX + kBarMaxWidth + 10.0f;
    // const float textY = kBoardY + kBarOffsetY + (kBarHeight - 24.0f) * 0.5f;
    // Renderer::DrawText(text, textX, textY);
}

// ------------------------------------------------------------------------------
// HP 取得
// - s_HP / s_MaxHP を安全に公開する
// - GetHP01 は [0, 1] 正規化値を返す（演出・UI で使用）
// ------------------------------------------------------------------------------
float HP::GetHP()    { return s_HP; }
float HP::GetMaxHP() { return s_MaxHP; }

float HP::GetHP01()
{
    if (s_MaxHP <= 0.0f) return 0.0f;
    return Clamp(s_HP / s_MaxHP, 0.0f, 1.0f);
}

bool HP::IsDead() { return s_HP <= 0.0f; }

// ------------------------------------------------------------------------------
// HP 設定
// - SetMaxHP は最低値を 1.0f に保証し、現在 HP を切り詰める
// - SetHP は [0, MaxHP] にクランプして代入する
// ------------------------------------------------------------------------------
void HP::SetMaxHP(float maxHp)
{
    s_MaxHP = std::max(1.0f, maxHp);
    s_HP    = Clamp(s_HP, 0.0f, s_MaxHP);
}

void HP::SetHP(float hp)
{
    s_HP = Clamp(hp, 0.0f, s_MaxHP);
}

// ------------------------------------------------------------------------------
// HP 操作
// - AddHP は正値で回復、負値でダメージ。結果を [0, MaxHP] にクランプする
// - Drain は deltaTime 秒分の減少を AddHP 経由で適用する
// ------------------------------------------------------------------------------
void HP::AddHP(float value)
{
    s_HP = Clamp(s_HP + value, 0.0f, s_MaxHP);
}

void HP::Drain(float deltaTime)
{
    AddHP(-s_DrainPerSec * deltaTime);
}

// ------------------------------------------------------------------------------
// イベントハンドラ
// - OnEnemyEnteredHole: HolePenalty 分だけ HP を減少させる
// - OnEnemyKilled     : KillHeal 分だけ HP を回復する
// ------------------------------------------------------------------------------
void HP::OnEnemyEnteredHole() { AddHP(-s_HolePenalty); }
void HP::OnEnemyKilled()      { AddHP(s_KillHeal); }

// ------------------------------------------------------------------------------
// パラメータ設定
// - ゲームバランス調整用。Init 後に外部から上書きできる
// ------------------------------------------------------------------------------
void HP::SetDrainPerSec(float drainPerSec) { s_DrainPerSec = drainPerSec; }
void HP::SetHolePenalty(float holePenalty) { s_HolePenalty = holePenalty; }
void HP::SetKillHeal(float killHeal)       { s_KillHeal    = killHeal; }
