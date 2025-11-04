#pragma once

#include "gameObject.h"
#include "audio.h"

class SoundManager : public GameObject
{
private:
    Audio m_SoundEffect; // 効果音用のAudioオブジェクト

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
};