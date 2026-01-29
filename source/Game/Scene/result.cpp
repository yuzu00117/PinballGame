// result.cpp
#include "main.h"
#include "result.h"
#include "renderer.h"
#include <windows.h>

void Result::Init() {
}

void Result::Uninit() {
}

void Result::Update(float deltaTime) {
}

void Result::Draw() {
    Renderer::DrawText(L"--- Result ---", 100, 200);
    Renderer::DrawText(L"Press Enter to Return to Title", 100, 280);
}
