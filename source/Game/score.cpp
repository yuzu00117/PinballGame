// score.cpp
#include "main.h"
#include "score.h"
#include "renderer.h"
#include <windows.h>
#include <string>

// �ÓI�����o�̒�`
int Score::s_Score = 0;

void Score::Init() {
    // �����������i�K�v�ɉ����āj
}

void Score::Uninit() {
    // �I�������i�K�v�ɉ����āj
}

void Score::Update(float deltaTime) {
    // �f�o�b�O�p�F+�L�[��-�L�[�ŃX�R�A����
    static bool prevPlus = false;
    static bool prevMinus = false;
    
    // +�L�[�iVK_OEM_PLUS �܂��� VK_ADD�j�̏���
    SHORT ksPlus = GetAsyncKeyState(VK_OEM_PLUS);
    bool currPlus = (ksPlus & 0x8000) != 0;
    if (currPlus && !prevPlus) {
        s_Score += 100;
        if (s_Score > 99999) s_Score = 99999; // 5������
    }
    prevPlus = currPlus;
    
    // -�L�[�iVK_OEM_MINUS �܂��� VK_SUBTRACT�j�̏���
    SHORT ksMinus = GetAsyncKeyState(VK_OEM_MINUS);
    bool currMinus = (ksMinus & 0x8000) != 0;
    if (currMinus && !prevMinus) {
        s_Score -= 100;
        if (s_Score < 0) s_Score = 0; // ���̒l�ɂ��Ȃ�
    }
    prevMinus = currMinus;
}

void Score::Draw() {
    // ��ʍ���ɃX�R�A��5���ŕ\��
    std::wstring scoreText = L"SCORE: " + std::to_wstring(s_Score);
    
    // 5���ɂȂ�悤��0���߂���
    std::wstring paddedScore = std::to_wstring(s_Score);
    while (paddedScore.length() < 5) {
        paddedScore = L"0" + paddedScore;
    }
    
    std::wstring displayText = L"SCORE: " + paddedScore;
    Renderer::DrawText(displayText, 10, 10); // ����ix=10, y=10�j�ɕ\��
}