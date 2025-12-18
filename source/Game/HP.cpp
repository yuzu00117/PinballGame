#include "HP.h"
#include "Renderer.h"
#include "Input.h"
#include <algorithm>
#include <string>
#include "MathUtil.h"

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
}

// 終了処理
void HP::Uninit()
{
    // 特にやることなし
}

// 更新処理
void HP::Update(float deltaTime)
{
    // 毎秒HP減少処理
    Drain(deltaTime);

#if defined(DEBUG)
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
    // --- HP表示 ---
    int hp  = static_cast<int>(s_HP);
    int max = static_cast<int>(s_MaxHP);
    
    std::wstring text = L"HP: " + std::to_wstring(hp) + L" / " + std::to_wstring(max);
    Renderer::DrawText(text, 10.0f, 30.0f);
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