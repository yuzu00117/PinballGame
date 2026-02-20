#include "Result.h"

// システム
#include "main.h"
#include "renderer.h"
#include "Input.h"
#include "GameManager.h"

//------------------------------------------------------------------------------
// 初期化処理
//------------------------------------------------------------------------------
// NOTE: 現状はリソース生成なし（未実装）
void Result::Init()
{
	GameObject::Init();
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
// - リザルト表示テキストを描画する
void Result::Draw()
{
	GameObject::Draw();

	Renderer::DrawText(L"--- Result ---", kResultX, kResultY);
	Renderer::DrawText(L"Press Enter to Return to Title", kPromptX, kPromptY);
}
