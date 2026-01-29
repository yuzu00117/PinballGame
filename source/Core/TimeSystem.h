#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class TimeSystem
{
public:
    // ------------------------------------------------------------------------------
    // 関数定義（Public）
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    static void Init();
    static void Update();

    /// <summary>
    /// 経過秒を取得
    /// DeltaTime       　: ゲーム時間（TimeScaleの影響を受ける）
    /// UnscaledDeltaTime : 実時間（TimeScaleの影響を受けない）
    /// ゲームの進みを制御するにはDeltaTimeを使用する
    /// ゲームが一時停止している間も動かしたい処理にはUnscaledDeltaTimeを使用する
    /// </summary>
    static float DeltaTime();         // 秒
    static float UnscaledDeltaTime(); // 秒（TimeScale無視）

    /// <summary>
    /// 時間の進み具合を設定・取得する
    /// 1.0f が通常速度、0.5f が半分の速度、2.0f が2倍の速度、0.0f が停止
    /// </summary>
    static void  SetTimeScale(float scale);

    /// <summary>
    /// 時間の進み具合を取得する
    /// </summary>
    static float TimeScale();

private:
    // ------------------------------------------------------------------------------
    // 変数定義（Private）
    // ------------------------------------------------------------------------------
    static LARGE_INTEGER s_Freq;  // 高精度タイマの周波数
    static LARGE_INTEGER s_Prev;  // 前フレームのカウンタ値
    static float s_UnscaledDelta; // 経過秒（TimeScale無視）
    static float s_Delta;         // 経過秒（TimeScale考慮）
    static float s_TimeScale;     // 時間の進み具合
};
