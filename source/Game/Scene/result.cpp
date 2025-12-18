// result.cpp
#include "main.h"
#include "result.h"
#include "renderer.h"
#include <windows.h>

void Result::Init() {
    // スコア取得やサウンド再生などの初期化があれば
}

void Result::Uninit() {
    // 後片付けがあれば
}

void Result::Update(float deltaTime) {
    // リトライ演出やアニメーションなど
}

void Result::Draw() {
    // 結果画面表示例
    Renderer::DrawText(L"--- Result ---", 100, 200);
    // スコアを表示する場合は GameManager か別クラスから取得して渡す
    // 例: Renderer::DrawText(scoreText, 100, 240);
    Renderer::DrawText(L"Press Enter to Return to Title", 100, 280);
}
