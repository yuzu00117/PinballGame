#include "TimeSystem.h"

// 標準ライブラリ
#include <algorithm>

// ------------------------------------------------------------------------------
// 静的メンバ定義
// ------------------------------------------------------------------------------
LARGE_INTEGER TimeSystem::s_Freq = {};
LARGE_INTEGER TimeSystem::s_Prev = {};
float         TimeSystem::s_UnscaledDelta = 0.0f;
float         TimeSystem::s_Delta = 0.0f;
float         TimeSystem::s_TimeScale = 1.0f;

// ------------------------------------------------------------------------------
// ライフサイクルメソッド
// ------------------------------------------------------------------------------
// - 高精度タイマを初期化する
// - 周波数と初回カウンタ値を取得する
void TimeSystem::Init()
{
    // 高精度タイマの周波数を取得
    QueryPerformanceFrequency(&s_Freq);

    // 初回のカウンタ値を取得
    QueryPerformanceCounter(&s_Prev);
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - 前フレームからの経過時間を計測する
// - 異常値をクランプし、DeltaTime / UnscaledDeltaTime を更新する
// NOTE:
// - デバッグ停止やウィンドウ非アクティブ時に極端な値が出るため、最大値を制限している
void TimeSystem::Update()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    const long long ticks = (now.QuadPart - s_Prev.QuadPart);
    s_Prev = now;

    // 経過秒（実時間）
    double dt = static_cast<double>(ticks) / static_cast<double>(s_Freq.QuadPart);

    // 異常値対策
    dt = std::clamp(dt, 0.0, 0.1); // 最大0.1秒までに制限

    s_UnscaledDelta = static_cast<float>(dt);
    s_Delta = s_UnscaledDelta * s_TimeScale;
}

// ------------------------------------------------------------------------------
// 経過秒を取得
// ------------------------------------------------------------------------------
// TimeScale を考慮した経過秒を取得する
float TimeSystem::DeltaTime()
{
    return s_Delta;
}

// ------------------------------------------------------------------------------
// 経過秒を取得（TimeScale無視）
// ------------------------------------------------------------------------------
// 実時間ベースの経過秒を取得する
float TimeSystem::UnscaledDeltaTime()
{
    return s_UnscaledDelta;
}

// ------------------------------------------------------------------------------
// 時間の進み具合を設定する
// ------------------------------------------------------------------------------
// - 0.0f 未満は許可しない
// - 0.0f の場合、時間は停止する
void TimeSystem::SetTimeScale(float scale)
{
    s_TimeScale = std::max(0.0f, scale);
}

// ------------------------------------------------------------------------------
// 時間の進み具合を取得する
// ------------------------------------------------------------------------------
// 現在の TimeScale 値を返す
float TimeSystem::TimeScale()
{
    return s_TimeScale;
}
