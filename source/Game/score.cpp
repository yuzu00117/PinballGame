// score.cpp
#include "main.h"
#include "score.h"
#include "renderer.h"
#include <windows.h>
#include <string>

int Score::s_Score = 0;

void Score::Init() {
}

void Score::Uninit() {
}

void Score::Update(float deltaTime) {
    static bool prevPlus = false;
    static bool prevMinus = false;
    
    SHORT ksPlus = GetAsyncKeyState(VK_OEM_PLUS);
    bool currPlus = (ksPlus & 0x8000) != 0;
    if (currPlus && !prevPlus) {
        s_Score += 100;
        if (s_Score > 99999) s_Score = 99999;
    }
    prevPlus = currPlus;
    
    SHORT ksMinus = GetAsyncKeyState(VK_OEM_MINUS);
    bool currMinus = (ksMinus & 0x8000) != 0;
    if (currMinus && !prevMinus) {
        s_Score -= 100;
        if (s_Score < 0) s_Score = 0;
    }
    prevMinus = currMinus;
}

void Score::Draw() {
    std::wstring scoreText = L"SCORE: " + std::to_wstring(s_Score);
    
    std::wstring paddedScore = std::to_wstring(s_Score);
    while (paddedScore.length() < 5) {
        paddedScore = L"0" + paddedScore;
    }
    
    std::wstring displayText = L"SCORE: " + paddedScore;
    Renderer::DrawText(displayText, 10, 10);
}
