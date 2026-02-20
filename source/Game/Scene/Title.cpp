#include "Title.h"

// システム
#include "main.h"
#include "renderer.h"

// Windows API
#include <windows.h>

//------------------------------------------------------------------------------
// 初期化処理
//------------------------------------------------------------------------------
// - 背景スプライトを子オブジェクトとして生成し、TitleBackGround.png を設定する
// - スプライトは画面全体（SCREEN_WIDTH × SCREEN_HEIGHT）を覆うサイズとする
void Title::Init()
{
	m_BgSprite = CreateChild<Sprite>();
	m_BgSprite->Init();
	m_BgSprite->SetTexture(kTitleBackGroundPath);
	m_BgSprite->SetPosition(0.0f, 0.0f);
	m_BgSprite->SetSize(
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT));
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
// NOTE: m_BgSprite は GameObject の子として管理されるため、個別解放は不要
void Title::Uninit()
{
}

//------------------------------------------------------------------------------
// 更新処理
//------------------------------------------------------------------------------
// TODO: Enterキー入力を検知し、次のシーンへ遷移する処理を実装する
void Title::Update(float deltaTime)
{
}

//------------------------------------------------------------------------------
// 描画処理
//------------------------------------------------------------------------------
// - 背景画像を先に描画し、その手前にテキストを重ねる
void Title::Draw()
{
	m_BgSprite->Draw();

	Renderer::DrawText(L"=== PinBall Battle ===", kTitleX, kTitleY);
	Renderer::DrawText(L"Press Enter to Start", kPromptX, kPromptY);
}
