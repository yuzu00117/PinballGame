// title.cpp
#include "main.h"
#include "title.h"
#include "renderer.h"
#include <windows.h>   // 表示位置計算などで利用する場合

void Title::Init()
{
    // 必要なら初期化。今回は特になし
}

void Title::Uninit()
{
    // 終了処理。今回は特になし
}

void Title::Update(float deltaTime)
{
    // タイトル画面固有の動作（アニメーション等）があればここに
}

void Title::Draw()
{
    // GameManager::Draw() 内で Begin()/End() しているため、
    // ここではテキスト描画のみ行う
    // 仮に Renderer::DrawText を使う場合の例：
    Renderer::DrawText(L"=== PinBall Battle ===", 100, 200);
    Renderer::DrawText(L"Press Enter to Start", 100, 240);
}
