// title.cpp
#include "main.h"
#include "title.h"
#include "renderer.h"
#include <windows.h>

void Title::Init()
{
}

void Title::Uninit()
{
}

void Title::Update(float deltaTime)
{
}

void Title::Draw()
{
    Renderer::DrawText(L"=== PinBall Battle ===", 100, 200);
    Renderer::DrawText(L"Press Enter to Start", 100, 240);
}
