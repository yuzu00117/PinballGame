// title.cpp
#include "main.h"
#include "title.h"
#include "renderer.h"
#include <windows.h>   // �\���ʒu�v�Z�Ȃǂŗ��p����ꍇ

void Title::Init()
{
    // �K�v�Ȃ珉�����B����͓��ɂȂ�
}

void Title::Uninit()
{
    // �I�������B����͓��ɂȂ�
}

void Title::Update(float deltaTime)
{
    // �^�C�g����ʌŗL�̓���i�A�j���[�V�������j������΂�����
}

void Title::Draw()
{
    // GameManager::Draw() ��� Begin()/End() ���Ă��邽�߁A
    // �����ł̓e�L�X�g�`��̂ݍs��
    // ���� Renderer::DrawText ��g���ꍇ�̗�F
    Renderer::DrawText(L"=== PinBall Battle ===", 100, 200);
    Renderer::DrawText(L"Press Enter to Start", 100, 240);
}
