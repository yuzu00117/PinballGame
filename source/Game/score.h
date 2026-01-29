#pragma once

#include "gameObject.h"

class Score : public GameObject
{
private:
    static int s_Score;

public:
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    static int GetScore() { return s_Score; }
    static void SetScore(int score) { s_Score = score; }
    static void AddScore(int points) { s_Score += points; }
};
