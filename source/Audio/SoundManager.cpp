// soundManager.cpp
#include "main.h"
#include "SoundManager.h"

// 静的メンバの定義
SoundManager* SoundManager::s_Instance = nullptr;

// 初期化処理
void SoundManager::Init()
{
    // シングルトンインスタンスの設定
    s_Instance = this;

    // --- 効果音の読み込み ---
    // ここでサウンドを読み込む
}

// 破棄処理
void SoundManager::Uninit()
{
    // 自分がインスタンスなら開放
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }

    // 登録済みサウンドの解放
    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Uninit();
    }
}

// 更新処理
void SoundManager::Update()
{
    // 特に更新処理はなし（音声管理なので）
    // 将来的には、フェード処理や一括ミュートなどをここで行うかも
}

// 描画処理
void SoundManager::Draw()
{
    // 描画処理は特になし（音声管理なので）
}

// サウンドの読み込み
void SoundManager::Load(SoundID id, const char* filename)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Load(filename);
}

// ------------------------------------------------------------------------------
// BGM・SEの再生 / 停止
// ------------------------------------------------------------------------------
// サウンドの再生
void SoundManager::Play(SoundID id, bool loop, float volume)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    float finalVolume = volume * m_MasterVolume;

    m_Sounds[index].SetVolume(finalVolume);
    m_Sounds[index].Play(loop);
}

// サウンドの停止
void SoundManager::Stop(SoundID id)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Stop();
}

// すべてのサウンドを停止
void SoundManager::StopAll()
{
    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Stop();
    }
}

// ------------------------------------------------------------------------------
// 音量設定
// ------------------------------------------------------------------------------
void SoundManager::SetMasterVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    m_MasterVolume = volume;
}