#include "Result.h"

// システム
#include "main.h"
#include "renderer.h"
#include "Input.h"
#include "GameManager.h"

//------------------------------------------------------------------------------
// 初期化処理
//------------------------------------------------------------------------------
// - 背景スプライトを子オブジェクトとして生成し、TitleBackGround.png を設定する
// - スプライトは画面全体（SCREEN_WIDTH x SCREEN_HEIGHT）を覆うサイズとする
void Result::Init()
{
	GameObject::Init();

	m_BgSprite = CreateChild<Sprite>();
	m_BgSprite->Init();
	m_BgSprite->SetTexture(kBgPath);
	m_BgSprite->SetPosition(0.0f, 0.0f);
	m_BgSprite->SetSize(
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT));
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
// NOTE: 現状はリソース解放なし（未実装）
void Result::Uninit()
{
	// TODO: m_VertexBuffer 等を使用する場合はここで Release する
	GameObject::Uninit();
}

//------------------------------------------------------------------------------
// 更新処理
//------------------------------------------------------------------------------
// - Enter キーが押された瞬間に Title シーンへ遷移する
void Result::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	if (Input::GetKeyTrigger(VK_RETURN))
	{
		GameManager::ChangeScene(GameManager::Scene::Title);
	}
}

//------------------------------------------------------------------------------
// 描画処理
//------------------------------------------------------------------------------
// - 背景画像を先に描画し、その手前にテキストを重ねる
void Result::Draw()
{
	m_BgSprite->Draw();

	Renderer::DrawText(L"--- Result ---", kResultX, kResultY);
	Renderer::DrawText(L"Press Enter to Return to Title", kPromptX, kPromptY);
}
