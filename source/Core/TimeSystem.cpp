#include "TimeSystem.h"
#include <algorithm>

LARGE_INTEGER TimeSystem::s_Freq = {};
LARGE_INTEGER TimeSystem::s_Prev = {};
float         TimeSystem::s_UnscaledDelta = 0.0f;
float         TimeSystem::s_Delta = 0.0f;
float         TimeSystem::s_TimeScale = 1.0f;

// ------------------------------------------------------------------------------
// ライフサイクルメソッド
// ------------------------------------------------------------------------------
// 初期化処理
void TimeSystem::Init()
{
    // 高精度タイマの周波数を取得
    QueryPerformanceFrequency(&s_Freq);

    // 初回のカウンタ値を取得
    QueryPerformanceCounter(&s_Prev);
}

// 更新処理
void TimeSystem::Update()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    const long long ticks = (now.QuadPart - s_Prev.QuadPart);
    s_Prev = now;

    double dt = (double)ticks / (double)s_Freq.QuadPart; // 経過秒（実時間）

    // 異常値対策（デバッグ停止・ウィンドウ非アクティブ化などで大きな値になることがある）
    dt = std::clamp(dt, 0.0, 0.1); // 最大0.1秒までに制限

    s_UnscaledDelta = (float)dt;
    s_Delta = s_UnscaledDelta * s_TimeScale;
}

// ------------------------------------------------------------------------------
// 経過秒を取得
// ------------------------------------------------------------------------------
// 経過秒（TimeScale考慮）
float TimeSystem::DeltaTime() { return s_Delta; }

// 経過秒（TimeScale無視）
float TimeSystem::UnscaledDeltaTime() { return s_UnscaledDelta; }

// ------------------------------------------------------------------------------
// 時間の進み具合を設定・取得する
// ------------------------------------------------------------------------------
// 時間の進み具合を取得する
void TimeSystem::SetTimeScale(float scale)
{
    s_TimeScale = std::max(0.0f, scale);
}

// 時間の進み具合を取得する
float TimeSystem::TimeScale() { return s_TimeScale; }