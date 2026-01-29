#include "score.h"

// システム
#include "main.h"
#include "renderer.h"

// Windows API / 標準ライブラリ
#include <windows.h>
#include <string>

// ------------------------------------------------------------------------------
// 静的メンバー定義
// ------------------------------------------------------------------------------
int Score::s_Score = 0;

// ------------------------------------------------------------------------------
// 初期化処理
// ------------------------------------------------------------------------------
void Score::Init()
{
    GameObject::Init();

    // スコア初期化
    s_Score = 0;
}

// ------------------------------------------------------------------------------
// 終了処理
// ------------------------------------------------------------------------------
void Score::Uninit()
{
    GameObject::Uninit();
}

// ------------------------------------------------------------------------------
// 更新処理
// ------------------------------------------------------------------------------
// - '+' キー押下：スコア加算（100）
// - '-' キー押下：スコア減算（100）
// - 上限/下限を表示仕様に合わせてクランプ
void Score::Update(float deltaTime)
{
    GameObject::Update(deltaTime);

    static bool prevPlus  = false;
    static bool prevMinus = false;

    // '+' 入力（加算）
    SHORT ksPlus = GetAsyncKeyState(VK_OEM_PLUS);
    bool currPlus = (ksPlus & 0x8000) != 0;
    if (currPlus && !prevPlus)
    {
        s_Score += 100;
        if (s_Score > 99999)
            s_Score = 99999;
    }
    prevPlus = currPlus;

    // '-' 入力（減算）
    SHORT ksMinus = GetAsyncKeyState(VK_OEM_MINUS);
    bool currMinus = (ksMinus & 0x8000) != 0;
    if (currMinus && !prevMinus)
    {
        s_Score -= 100;
        if (s_Score < 0)
            s_Score = 0;
    }
    prevMinus = currMinus;
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - スコアを5桁固定（ゼロ埋め）で左上に描画
void Score::Draw()
{
    GameObject::Draw();

    std::wstring paddedScore = std::to_wstring(s_Score);
    while (paddedScore.length() < 5)
    {
        paddedScore = L"0" + paddedScore;
    }

    const std::wstring displayText = L"SCORE: " + paddedScore;
    Renderer::DrawText(displayText, 10, 10);
}
