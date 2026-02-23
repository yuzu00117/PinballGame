#pragma once

#include "gameObject.h"
#include <memory>
#include "../Graphics/Sprite.h"

/// <summary>
/// HP管理クラス
/// </summary>
class HP : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // 関数定義（Public）
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// HP情報を取得
    /// </summary>
    static float GetHP();                // 現在HPを取得
    static float GetMaxHP();             // 最大HPを取得
    static float GetHP01();              // HPを0～1の範囲で取得（演出などで使用）
    static bool  IsDead();               // 死亡判定

    /// <summary>
    /// HP情報を設定
    /// </summary>
    static void SetMaxHP(float maxHp);   // 最大HP設定
    static void SetHP(float hp);         // 現在HP設定

    /// <summary>
    /// HP操作
    /// </summary>
    static void AddHP(float value);      // +回復 / -ダメージ
    static void Drain(float deltaTime);  // 毎秒減少

    /// <summary>
    /// イベント処理
    /// </summary>
    static void OnEnemyEnteredHole();    // エネミーがホールに入った
    static void OnEnemyKilled();         // エネミー撃破

    /// <summary>
    /// パラメータ設定
    /// </summary>
    static void SetDrainPerSec(float v); 
    static void SetHolePenalty(float v);
    static void SetKillHeal(float v);

private:
    // ------------------------------------------------------------------------------
    // 定数定義（Private）
    // ------------------------------------------------------------------------------
    static constexpr float kDefaultMaxHP       = 100.0f; // デフォルト最大HP
    static constexpr float kDefaultDrainPerSec = 1.0f;   // デフォルト毎秒HP減少量
    static constexpr float kDefaultHolePenalty = 10.0f;  // デフォルトのホールペナルティ
    static constexpr float kDefaultKillHeal    = 5.0f;   // デフォルトの撃破回復量

    // HPボード画像の表示サイズ
    static constexpr float kBoardWidth  = 560.0f; // HPボードの表示幅
    static constexpr float kBoardHeight =  70.0f; // HPボードの表示高さ

    // 画面下中央に配置する座標（左上原点）
    static constexpr float kBoardX = (1280.0f - kBoardWidth)  / 2.0f; // 画面水平中央
    static constexpr float kBoardY =  720.0f - kBoardHeight - 10.0f;   // 画面底から少し上

    // HPバーの表示サイズ・オフセット（ボード内の位置）
    static constexpr float kBarOffsetX  =  80.0f;  // ボード左端からの余白
    static constexpr float kBarOffsetY  =  20.0f;  // ボード上端からの余白
    static constexpr float kBarMaxWidth = 355.0f;  // HP100% 時のバー幅
    static constexpr float kBarHeight   =  20.0f;  // バーの高さ

    // ------------------------------------------------------------------------------
    // 変数定義（Private）
    // インスタンス化しないので、staticメンバで管理
    // ------------------------------------------------------------------------------
    // HP情報
    static float s_HP;                                   // 現在HP
    static float s_MaxHP;                                // 最大HP

    // パラメータ
    static float s_DrainPerSec;                          // 毎秒のHP減少量
    static float s_HolePenalty;                          // 敵がホールに入ったときのペナルティ
    static float s_KillHeal;                             // 敵を倒したときの回復量

    // HPボード・HPバー スプライト
    std::unique_ptr<Sprite> m_BoardSprite; // 所有：HPボード背景画像
    std::unique_ptr<Sprite> m_BarSprite;   // 所有：HPバースプライト（テクスチャなし）
};
