// soundManager.cpp
#include "main.h"
#include "soundManager.h"
#include "renderer.h"
#include <windows.h>

void SoundManager::Init() {
    // 効果音ファイルをロード
    m_SoundEffect.Load("asset/sounds/wan.wav");
}

void SoundManager::Uninit() {
    // オーディオリソースの解放
    m_SoundEffect.Uninit();
}

void SoundManager::Update() {
    // Pキーで効果音を再生
    static bool prevP = false;
    
    SHORT ksP = GetAsyncKeyState('P');
    bool currP = (ksP & 0x8000) != 0;
    if (currP && !prevP) {
        // Pキーが押された瞬間に効果音を再生
        m_SoundEffect.Play(false); // ループなしで再生
    }
    prevP = currP;
}

void SoundManager::Draw() {
    // 描画処理は特になし（音声管理なので）
}