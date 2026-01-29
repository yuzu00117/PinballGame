#include "main.h"
#include "SoundManager.h"

#include <algorithm>

SoundManager* SoundManager::s_Instance = nullptr;

// ------------------------------------------------------------------------------
// 初期化
// ------------------------------------------------------------------------------
// - グローバル参照を this に設定する
void SoundManager::Init()
{
    s_Instance = this;
}

// ------------------------------------------------------------------------------
// 終了
// ------------------------------------------------------------------------------
// - 自分がグローバル参照なら nullptr に戻す
// - すべての Audio を Uninit する
void SoundManager::Uninit()
{
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }

    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Uninit();
    }
}

// ------------------------------------------------------------------------------
// 更新
// ------------------------------------------------------------------------------
// NOTE: 現状は未使用（フェード等を実装した場合にここで更新する）
void SoundManager::Update(float /*deltaTime*/)
{
}

// ------------------------------------------------------------------------------
// 描画
// ------------------------------------------------------------------------------
// NOTE: 音声は描画しないため空
void SoundManager::Draw()
{
}

// ------------------------------------------------------------------------------
// 読み込み
// ------------------------------------------------------------------------------
// - id が範囲外の場合は何もしない
void SoundManager::Load(SoundID id, const char* filename)
{
    const int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Load(filename);
}

// ------------------------------------------------------------------------------
// 再生
// ------------------------------------------------------------------------------
// - 最終音量 = volume * m_MasterVolume
// - id が範囲外の場合は何もしない
void SoundManager::Play(SoundID id, bool loop, float volume)
{
    const int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    const float finalVolume = volume * m_MasterVolume;

    m_Sounds[index].SetVolume(finalVolume);
    m_Sounds[index].Play(loop);
}

// ------------------------------------------------------------------------------
// 停止
// ------------------------------------------------------------------------------
void SoundManager::Stop(SoundID id)
{
    const int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Stop();
}

// ------------------------------------------------------------------------------
// 全停止
// ------------------------------------------------------------------------------
void SoundManager::StopAll()
{
    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Stop();
    }
}

// ------------------------------------------------------------------------------
// マスターボリューム設定
// ------------------------------------------------------------------------------
// - 0.0〜1.0 にクランプする
void SoundManager::SetMasterVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    m_MasterVolume = volume;
}
