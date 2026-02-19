// title.cpp
#include "title.h"

// システム
#include "main.h"
#include "renderer.h"

// Windows API
#include <windows.h>

// 初期化処理
void Title::Init()
{
}

// 終了処理
void Title::Uninit()
{
}

// 更新処理
void Title::Update(float deltaTime)
{
}

// 描画処理
void Title::Draw()
{
    Renderer::DrawText(L"=== PinBall Battle ===", kTitleX, kTitleY);
    Renderer::DrawText(L"Press Enter to Start", kPromptX, kPromptY);
}
