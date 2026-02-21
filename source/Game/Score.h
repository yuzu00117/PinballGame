//------------------------------------------------------------------------------
// Score.h
//------------------------------------------------------------------------------
// 役割:
// ゲーム中のスコアを管理・表示する GameObject。
// 現在はデバッグ用途としてキー入力によりスコアを増減できる。
//
// 設計意図:
// スコアはシーンを跨いで共有されるため static 変数として保持する。
// 表示は Draw() に集約し、更新（加算/減算・クランプ）は Update() で行う。
// 将来的には GameManager 等へ責務を移すことも想定している。
//
// 構成:
// - スコア管理             : static int s_Score
// - 更新処理               : デバッグキー入力（+ / -）
// - 描画                   : 5桁固定のスコア文字列を描画
//
// NOTE:
// - 現在のキー入力処理はデバッグ用途
// - GetAsyncKeyState に依存（Windows API）
//------------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include <memory>
#include "../Graphics/Sprite.h"
#include "../Graphics/SpriteSheet.h"
/// スコア管理・表示用 GameObject
/// - スコアは static 変数として全体で共有される
/// - Draw で常に現在のスコアを表示する
class Score : public GameObject
{
public:
	//------------------------------------------------------------------------------
	// 定数定義
	//------------------------------------------------------------------------------
	static constexpr int kScoreIncrement = 100;
	static constexpr int kMaxScore = 99999;
	static constexpr int kMinScore = 0;

	static constexpr float kScoreBoardWidth = 384.0f;
	static constexpr float kScoreBoardHeight = 256.0f;

	// デフォルトのスコアボードのサイズ
	static constexpr float kDefaultScoreBoardSizeX = kScoreBoardWidth * 1.5f;
	static constexpr float kDefaultScoreBoardSizeY = kScoreBoardHeight * 1.5f;

	// 画面中央上部にUI固定配置する
	static constexpr float kScoreBoardX = SCREEN_WIDTH / 2.0f - kDefaultScoreBoardSizeX / 2.0f;
	static constexpr float kScoreBoardY = -150.0f;

	static constexpr int kScoreTextX = 10;
	static constexpr int kScoreTextY = 10;

	// スコアのスプライト表示用パラメータ
	static constexpr float kScoreNumberWidth = 24.0f;  	// 1桁の幅
	static constexpr float kScoreNumberHeight = 36.0f; 	// 1桁の高さ
	static constexpr float kScoreNumberScale = 0.8f;    // 拡大率（少し小さめ）
	static constexpr float kScoreNumberSpacing = 20.0f; // 桁間のスペース
	
	// スコアボード上での数字の描画開始位置（左上基準）
	static constexpr float kScoreNumberOffsetX = 310.0f;
	static constexpr float kScoreNumberOffsetY = 172.0f;

	//------------------------------------------------------------------------------
	// ライフサイクルメソッド
	//------------------------------------------------------------------------------
	Score();
	~Score() override;

	/// 初期化処理
	void Init() override;

	/// 終了処理
	void Uninit() override;

	/// 更新処理（deltaTime は秒単位）
	/// - デバッグ入力によりスコアを増減する
	void Update(float deltaTime) override;

	/// 描画処理
	/// - 左上にスコアを5桁固定で描画する
	void Draw() override;

	//------------------------------------------------------------------------------
	// スコア操作
	//------------------------------------------------------------------------------
	/// 現在のスコアを取得する
	static int GetScore() { return s_Score; }

	/// スコアを設定する
	static void SetScore(int score) { s_Score = score; }

	/// スコアを加算する
	static void AddScore(int points) { s_Score += points; }

private:
	//------------------------------------------------------------------------------
	// スコア管理
	//------------------------------------------------------------------------------
	static int s_Score;   // 共有スコア（全シーン共通）

	std::unique_ptr<Sprite>      m_ScoreBoardSprite; // 所有：スコアボード画像スプライト
	std::unique_ptr<SpriteSheet> m_NumberSprite;     // 所有：スコア数値のスプライトシート
};
