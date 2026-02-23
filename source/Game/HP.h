//------------------------------------------------------------------------------
// HP
//------------------------------------------------------------------------------
// 役割:
// プレイヤーの HP を一元管理し、HPバーとボード画像を描画する
//
// 設計意図:
// HP値はゲーム全体で共有する単一状態のため、static メンバで保持する
// GameObject を継承することで描画・更新ループに自然に組み込める
//
// 構成:
// - HP値・パラメータの取得 / 設定インタフェース
// - 毎フレーム HP 減少（Drain）処理
// - HPボード画像と HPバースプライトの描画
//
// NOTE:
// インスタンスは1つだけ生成すること
// static メンバを直接参照するため、複数インスタンスは想定外
//------------------------------------------------------------------------------
#pragma once

#include "gameObject.h"
#include <memory>
#include "../Graphics/Sprite.h"

/// HP を管理し、HPバーを描画するクラス
/// - プレイヤー HP の単一ソース・オブ・トゥルースとして機能する
/// - static メンバで HP 状態を保持するため、インスタンスは1つまで
class HP : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // ライフサイクル
    // ------------------------------------------------------------------------------
    /// 初期化処理
    /// - HP・各パラメータをデフォルト値でリセットする
    /// - HPボードスプライトと HPバースプライトを生成する
    void Init() override;

    /// 終了処理
    /// - 所有スプライトを Uninit して解放する
    void Uninit() override;

    /// 更新処理
    /// - 毎フレーム Drain を呼び出して HP を減少させる
    /// - HPバーの幅・色を現在の HP 比率に追従させる
    void Update(float deltaTime) override;

    /// 描画処理
    /// - HPボード画像を描画する
    /// - HPバースプライトを描画する
    void Draw() override;

    // ------------------------------------------------------------------------------
    // HP 取得
    // ------------------------------------------------------------------------------
    /// 現在 HP の値を返す
    static float GetHP();

    /// 最大 HP の値を返す
    static float GetMaxHP();

    /// 現在 HP を [0, 1] に正規化した値を返す
    /// - 最大 HP が 0 以下のとき 0.0f を返す
    static float GetHP01();

    /// HP が 0 以下のとき true を返す
    static bool IsDead();

    // ------------------------------------------------------------------------------
    // HP 設定
    // ------------------------------------------------------------------------------
    /// 最大 HP を設定する
    /// - 最低値は 1.0f にクランプされる
    /// - 現在 HP が新しい最大 HP を超える場合は切り詰められる
    static void SetMaxHP(float maxHp);

    /// 現在 HP を設定する
    /// - [0, MaxHP] にクランプされる
    static void SetHP(float hp);

    // ------------------------------------------------------------------------------
    // HP 操作
    // ------------------------------------------------------------------------------
    /// HP を増減する（正値で回復、負値でダメージ）
    /// - 結果は [0, MaxHP] にクランプされる
    static void AddHP(float value);

    /// deltaTime 秒分だけ HP を減少させる
    static void Drain(float deltaTime);

    // ------------------------------------------------------------------------------
    // イベントハンドラ
    // ------------------------------------------------------------------------------
    /// エネミーがホールに入ったときに呼び出す
    /// - HolePenalty 分だけ HP を減少させる
    static void OnEnemyEnteredHole();

    /// エネミーを撃破したときに呼び出す
    /// - KillHeal 分だけ HP を回復する
    static void OnEnemyKilled();

    // ------------------------------------------------------------------------------
    // パラメータ設定
    // ------------------------------------------------------------------------------
    /// 毎秒の HP 減少量を設定する
    static void SetDrainPerSec(float drainPerSec);

    /// エネミーがホールに入ったときの HP ペナルティ量を設定する
    static void SetHolePenalty(float holePenalty);

    /// エネミーを撃破したときの HP 回復量を設定する
    static void SetKillHeal(float killHeal);

private:
    // ------------------------------------------------------------------------------
    // 定数
    // ------------------------------------------------------------------------------
    static constexpr float kDefaultMaxHP       = 100.0f; // デフォルト最大HP
    static constexpr float kDefaultDrainPerSec =   1.0f; // デフォルト毎秒HP減少量
    static constexpr float kDefaultHolePenalty =  10.0f; // デフォルトホールペナルティ
    static constexpr float kDefaultKillHeal    =   5.0f; // デフォルト撃破回復量

    // HPボード画像の表示サイズ
    static constexpr float kBoardWidth  = 560.0f; // ボードの表示幅
    static constexpr float kBoardHeight =  70.0f; // ボードの表示高さ

    // 画面下中央に配置する座標（左上原点）
    static constexpr float kBoardX = (1280.0f - kBoardWidth) / 2.0f; // 水平中央
    static constexpr float kBoardY = 720.0f - kBoardHeight - 10.0f;  // 画面底から上

    // HPバーの表示サイズ・ボード内オフセット
    static constexpr float kBarOffsetX  =  80.0f; // ボード左端からの余白
    static constexpr float kBarOffsetY  =  20.0f; // ボード上端からの余白
    static constexpr float kBarMaxWidth = 355.0f; // HP 100% 時のバー幅
    static constexpr float kBarHeight   =  20.0f; // バーの高さ

    // ------------------------------------------------------------------------------
    // メンバ変数
    // NOTE: HP 状態は static で保持。インスタンスは描画スプライトのみ所有する
    // ------------------------------------------------------------------------------
    static float s_HP;           // 現在HP
    static float s_MaxHP;        // 最大HP
    static float s_DrainPerSec;  // 毎秒のHP減少量
    static float s_HolePenalty;  // エネミーがホールに入ったときのペナルティ
    static float s_KillHeal;     // エネミーを撃破したときの回復量

    std::unique_ptr<Sprite> m_BoardSprite; // 所有：HPボード背景画像
    std::unique_ptr<Sprite> m_BarSprite;   // 所有：HPバースプライト（テクスチャなし・色のみ）
};
