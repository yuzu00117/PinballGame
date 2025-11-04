#pragma once

#include "gameObject.h"

class Score : public GameObject
{
private:
    static int s_Score; // 静的スコア変数

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // スコア操作用の静的メソッド
    static int GetScore() { return s_Score; }
    static void SetScore(int score) { s_Score = score; }
    static void AddScore(int points) { s_Score += points; }
};