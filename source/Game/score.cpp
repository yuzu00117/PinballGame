// score.cpp
#include "main.h"
#include "score.h"
#include "renderer.h"
#include <windows.h>
#include <string>

// 静的メンバの定義
int Score::s_Score = 0;

void Score::Init() {
    // 初期化処理（必要に応じて）
}

void Score::Uninit() {
    // 終了処理（必要に応じて）
}

void Score::Update(float deltaTime) {
    // デバッグ用：+キーと-キーでスコア増減
    static bool prevPlus = false;
    static bool prevMinus = false;
    
    // +キー（VK_OEM_PLUS または VK_ADD）の処理
    SHORT ksPlus = GetAsyncKeyState(VK_OEM_PLUS);
    bool currPlus = (ksPlus & 0x8000) != 0;
    if (currPlus && !prevPlus) {
        s_Score += 100;
        if (s_Score > 99999) s_Score = 99999; // 5桁制限
    }
    prevPlus = currPlus;
    
    // -キー（VK_OEM_MINUS または VK_SUBTRACT）の処理
    SHORT ksMinus = GetAsyncKeyState(VK_OEM_MINUS);
    bool currMinus = (ksMinus & 0x8000) != 0;
    if (currMinus && !prevMinus) {
        s_Score -= 100;
        if (s_Score < 0) s_Score = 0; // 負の値にしない
    }
    prevMinus = currMinus;
}

void Score::Draw() {
    // 画面左上にスコアを5桁で表示
    std::wstring scoreText = L"SCORE: " + std::to_wstring(s_Score);
    
    // 5桁になるように0埋めする
    std::wstring paddedScore = std::to_wstring(s_Score);
    while (paddedScore.length() < 5) {
        paddedScore = L"0" + paddedScore;
    }
    
    std::wstring displayText = L"SCORE: " + paddedScore;
    Renderer::DrawText(displayText, 10, 10); // 左上（x=10, y=10）に表示
}