// result.cpp
#include "main.h"
#include "result.h"
#include "renderer.h"
#include <windows.h>

void Result::Init() {
    // �X�R�A�擾��T�E���h�Đ��Ȃǂ̏������������
}

void Result::Uninit() {
    // ��Еt���������
}

void Result::Update(float deltaTime) {
    // ���g���C���o��A�j���[�V�����Ȃ�
}

void Result::Draw() {
    // ���ʉ�ʕ\����
    Renderer::DrawText(L"--- Result ---", 100, 200);
    // �X�R�A��\������ꍇ�� GameManager ���ʃN���X����擾���ēn��
    // ��: Renderer::DrawText(scoreText, 100, 240);
    Renderer::DrawText(L"Press Enter to Return to Title", 100, 280);
}
