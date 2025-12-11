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
void Load(SoundID id, const char* filename)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) { return; }

    m_Sounds[index].Load(filename);
}

// ------------------------------------------------------------------------------
// BGM・SEの再生
// ------------------------------------------------------------------------------
void SoundManager::Play(BGMID id, bool loop, float volume)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)BGMID::Count) return;

    float finalVolume = volume * m_BGMVolume;

    m_BGMs[index].SetVolume(finalVolume);
    m_BGMs[index].Play(loop);
}

void SoundManager::Play(SEID id, bool loop, float volume)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SEID::Count) return;

    float finalVolume = volume * m_SEVolume;

    m_SEs[index].SetVolume(finalVolume);
    m_SEs[index].Play(loop);
}

// ------------------------------------------------------------------------------
// 音量設定
// ------------------------------------------------------------------------------
void SoundManager::SetBGMVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    m_BGMVolume = volume;

    // ここで現在再生中のBGMにも適用する場合は
    // m_BGMs配列をループしてSetVolumeを呼び出す
}

void SoundManager::SetSEVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    m_SEVolume = volume;

    // ここで現在再生中のSEにも適用する場合は
    // m_SEs配列をループしてSetVolumeを呼び出す
}