#pragma once

#include "gameObject.h"

class Score : public GameObject
{
private:
    static int s_Score; // �ÓI�X�R�A�ϐ�

public:
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    // �X�R�A����p�̐ÓI���\�b�h
    static int GetScore() { return s_Score; }
    static void SetScore(int score) { s_Score = score; }
    static void AddScore(int points) { s_Score += points; }
};