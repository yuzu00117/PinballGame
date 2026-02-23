#include "HP.h"
#include "Renderer.h"
#include "Input.h"
#include <algorithm>
#include <string>
#include "MathUtil.h"
#include "../Graphics/Sprite.h"

// --------------------------------------------------------------------------------
// Staticメンバ変数定義
// --------------------------------------------------------------------------------
float HP::s_HP          = 0.0f; // 現在HP
float HP::s_MaxHP       = 0.0f; // 最大HP
float HP::s_DrainPerSec = 0.0f; // 毎秒のHP減少量
float HP::s_HolePenalty = 0.0f; // 敵がホールに入ったときのペナルティ
float HP::s_KillHeal    = 0.0f; // 敵を倒したときの回復量

// --------------------------------------------------------------------------------
// ライフサイクルメソッド
// --------------------------------------------------------------------------------
// 初期化処理
void HP::Init()
{
    // --- パラメータ初期化 ---
    // HP情報の初期化
    s_MaxHP       = kDefaultMaxHP;       // 最大HP初期化
    s_HP          = s_MaxHP;             // 現在HP初期化

    // パラメータ初期化
    s_DrainPerSec = kDefaultDrainPerSec; // 毎秒のHP減少量
    s_HolePenalty = kDefaultHolePenalty; // 敵がホールに入ったときのペナルティ
    s_KillHeal    = kDefaultKillHeal;    // 敵を倒したときの回復量

    // --- HPボード画像の初期化 ---
    m_BoardSprite = std::make_unique<Sprite>();
    m_BoardSprite->Init();
    m_BoardSprite->SetTexture(L"asset/texture/HPBoard.png");
    m_BoardSprite->SetSize(kBoardWidth, kBoardHeight);
    m_BoardSprite->SetPosition(kBoardX, kBoardY);

    // --- HPバースプライトの初期化 ---
    // NOTE: テクスチャなし・SetColor のみで色塗りする
    m_BarSprite = std::make_unique<Sprite>();
    m_BarSprite->Init();
    m_BarSprite->SetSize(kBarMaxWidth, kBarHeight);
    m_BarSprite->SetPosition(kBoardX + kBarOffsetX, kBoardY + kBarOffsetY);
    m_BarSprite->SetColor(0.2f, 0.9f, 0.2f); // 初期色：緑
}

// 終了処理
void HP::Uninit()
{
    // HPボード画像の解放
    if (m_BoardSprite)
    {
        m_BoardSprite->Uninit();
        m_BoardSprite.reset();
    }

    // HPバースプライトの解放
    if (m_BarSprite)
    {
        m_BarSprite->Uninit();
        m_BarSprite.reset();
    }
}

// 更新処理
void HP::Update(float deltaTime)
{
    // 毎秒HP減少処理
    Drain(deltaTime);

    // --- HPバー幅・色の更新 ---
    if (m_BarSprite)
    {
        const float ratio    = GetHP01();
        const float barWidth = kBarMaxWidth * ratio;
        m_BarSprite->SetSize(barWidth, kBarHeight);

        // HP割合に応じて色変化（段階切替）
        if      (ratio >= 0.6f) { m_BarSprite->SetColor(0.2f,  0.9f,  0.2f); }   // 緑
        else if (ratio >= 0.3f) { m_BarSprite->SetColor(0.95f, 0.85f, 0.1f); }   // 黄
        else                    { m_BarSprite->SetColor(0.9f,  0.2f,  0.2f); }   // 赤
    }

#if defined(_DEBUG)
    // --- デバッグ操作 ---
    // H: 回復 +10
    // J: ダメージ -10
    // K: エネミーがホールに入った
    // L: エネミー撃破
    if (Input::GetKeyTrigger('H')) { AddHP(10.0f); }
    if (Input::GetKeyTrigger('J')) { AddHP(-10.0f); }
    if (Input::GetKeyTrigger('K')) { OnEnemyEnteredHole(); }
    if (Input::GetKeyTrigger('L')) { OnEnemyKilled(); }
#endif
}

// 描画処理
void HP::Draw()
{
    // --- HPボード画像の描画 ---
    if (m_BoardSprite)
    {
        m_BoardSprite->Draw();
    }

    // --- HPバーの描画 ---
    if (m_BarSprite)
    {
        m_BarSprite->Draw();
    }

    // --- HPテキスト表示（数字のみ・バーの右隣に配置）---
    // int hp = static_cast<int>(s_HP);

    // std::wstring text = std::to_wstring(hp);

    // // バー右端の10px右・バーと縦中央揃え（フォントサイズ24基準）
    // const float textX = kBoardX + kBarOffsetX + kBarMaxWidth + 10.0f;
    // const float textY = kBoardY + kBarOffsetY + (kBarHeight - 24.0f) * 0.5f;
    // Renderer::DrawText(text, textX, textY);
}

// --------------------------------------------------------------------------------
// HP情報の取得
// --------------------------------------------------------------------------------
// 現在HPを取得
float HP::GetHP() { return s_HP; }

// 最大HPを取得
float HP::GetMaxHP() { return s_MaxHP; }

// HPを0～1の範囲で取得（演出などで使用）
float HP::GetHP01()
{
    if (s_MaxHP <= 0.0f) return 0.0f;
    return Clamp(s_HP / s_MaxHP, 0.0f, 1.0f);
}

// 死亡判定
bool HP::IsDead()
{
    return s_HP <= 0.0f;
}

// --------------------------------------------------------------------------------
// HP情報の設定
// --------------------------------------------------------------------------------
// 最大HP設定
void HP::SetMaxHP(float maxHp)
{
    s_MaxHP = std::max(1.0f, maxHp);
    s_HP = Clamp(s_HP, 0.0f, s_MaxHP);
}

// 現在HP設定
void HP::SetHP(float hp)
{
    s_HP = Clamp(hp, 0.0f, s_MaxHP);
}

// --------------------------------------------------------------------------------
// HP操作
// +回復 / -ダメージ
// --------------------------------------------------------------------------------
// HP増減
void HP::AddHP(float value)
{
    s_HP = Clamp(s_HP + value, 0.0f, s_MaxHP);
}

// 毎秒減少
void HP::Drain(float deltaTime)
{
    AddHP(-s_DrainPerSec * deltaTime);
}

// --------------------------------------------------------------------------------
// イベント処理
// 他のクラスから呼び出す用
// --------------------------------------------------------------------------------
// エネミーがホールに入った
void HP::OnEnemyEnteredHole()
{
    AddHP(-s_HolePenalty);
}

// エネミー撃破
void HP::OnEnemyKilled()
{
    AddHP(s_KillHeal);
}

// --------------------------------------------------------------------------------
// パラメータ設定
// --------------------------------------------------------------------------------
// 毎秒のHP減少量設定
void HP::SetDrainPerSec(float drainPerSec) { s_DrainPerSec = drainPerSec; }

// 敵がホールに入ったときのペナルティ設定
void HP::SetHolePenalty(float holePenalty) { s_HolePenalty = holePenalty; }

// 敵を倒したときの回復量設定
void HP::SetKillHeal(float killHeal) { s_KillHeal = killHeal; }
