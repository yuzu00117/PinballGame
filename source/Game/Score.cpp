#include "Score.h"

// システム
#include "main.h"
#include "renderer.h"

// GameObject
#include "Sprite.h"
#include "SpriteSheet.h"

// Windows API / 標準ライブラリ
#include <windows.h>
#include <string>

//------------------------------------------------------------------------------
// 静的メンバ定義
//------------------------------------------------------------------------------
int Score::s_Score = 0;

Score::Score() = default;
Score::~Score() = default;

//------------------------------------------------------------------------------
// 初期化処理
//------------------------------------------------------------------------------
void Score::Init()
{
	GameObject::Init();

	// スコア初期化
	s_Score = 0;

	// スコアボードの準備
	m_ScoreBoardSprite = std::make_unique<Sprite>();
	m_ScoreBoardSprite->Init();
	m_ScoreBoardSprite->SetTexture(L"asset/texture/ScoreBoard.png");
	m_ScoreBoardSprite->SetSize(kDefaultScoreBoardSizeX, kDefaultScoreBoardSizeY);
	m_ScoreBoardSprite->SetPosition(kScoreBoardX, kScoreBoardY);

	// 数値スプライトの準備
	m_NumberSprite = std::make_unique<SpriteSheet>();
	m_NumberSprite->Init();
	m_NumberSprite->SetTexture(L"asset/texture/ScoreNumber.png");
	// 数字は 0~9 まで横に10分割されている想定
	m_NumberSprite->SetGrid(10, 1);
	m_NumberSprite->SetSize(kScoreNumberWidth * kScoreNumberScale, kScoreNumberHeight * kScoreNumberScale);
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
void Score::Uninit()
{
	if (m_ScoreBoardSprite)
	{
		m_ScoreBoardSprite->Uninit();
		m_ScoreBoardSprite.reset();
	}

	if (m_NumberSprite)
	{
		m_NumberSprite->Uninit();
		m_NumberSprite.reset();
	}

	GameObject::Uninit();
}

//------------------------------------------------------------------------------
// 更新処理
//------------------------------------------------------------------------------
// - '+' キー押下：スコア加算（100）
// - '-' キー押下：スコア減算（100）
// - 上限/下限を表示仕様に合わせてクランプ
void Score::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	if (m_ScoreBoardSprite)
	{
		m_ScoreBoardSprite->Update(deltaTime);
	}

	static bool prevPlus  = false;
	static bool prevMinus = false;

	// '+' 入力（加算）
	SHORT ksPlus = GetAsyncKeyState(VK_OEM_PLUS);
	bool currPlus = (ksPlus & 0x8000) != 0;
	if (currPlus && !prevPlus)
	{
		s_Score += kScoreIncrement;
		if (s_Score > kMaxScore)
			s_Score = kMaxScore;
	}
	prevPlus = currPlus;

	// '-' 入力（減算）
	SHORT ksMinus = GetAsyncKeyState(VK_OEM_MINUS);
	bool currMinus = (ksMinus & 0x8000) != 0;
	if (currMinus && !prevMinus)
	{
		s_Score -= kScoreIncrement;
		if (s_Score < kMinScore)
			s_Score = kMinScore;
	}
	prevMinus = currMinus;
}

//------------------------------------------------------------------------------
// 描画処理
//------------------------------------------------------------------------------
// - スコアを5桁固定（ゼロ埋め）で左上に描画
void Score::Draw()
{
	GameObject::Draw();

	if (m_ScoreBoardSprite)
	{
		m_ScoreBoardSprite->Draw();
	}

	if (m_NumberSprite)
	{
		// 5桁のゼロ埋め文字列を取得
		std::wstring paddedScore = std::to_wstring(s_Score);
		while (paddedScore.length() < 5)
		{
			paddedScore = L"0" + paddedScore;
		}

		// 描画開始位置（スコアボードからの相対位置を反映）
		float startX = kScoreBoardX + kScoreNumberOffsetX;
		float startY = kScoreBoardY + kScoreNumberOffsetY;

		// 各桁を描画
		for (size_t i = 0; i < paddedScore.length(); ++i)
		{
			// 文字 ('0'~'9') からインデックス (0~9) に変換
			int numIndex = paddedScore[i] - L'0';

			// 位置とインデックスを設定して描画
			float posX = startX + (i * kScoreNumberSpacing);
			m_NumberSprite->SetPosition(posX, startY);
			m_NumberSprite->SetIndex(numIndex);
			m_NumberSprite->Draw();
		}
	}
}
