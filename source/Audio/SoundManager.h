#pragma once

#include "GameObject.h"
#include "Audio.h"
#include "SoundID.h"

/// <summary>
/// サウンド管理クラス
/// TODO: 将来的には、UnityのAudioManagerのように
/// BGMやSEの一括管理、フェードイン・フェードアウトを実装したい
/// </summary>
class SoundManager : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // 関数定義
    // ------------------------------------------------------------------------------
    /// <summary>
    /// グローバルインスタンスの取得
    /// </summary>
    static SoundManager& GetInstance() { return *s_Instance; }
        
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// サウンドの読み込み
    /// </summary>
    void Load(SoundID id, const char* filename);

    /// <summary>
    /// BGM・SEの再生・停止
    /// </summary>
    void Play(SoundID id, bool loop = false, float volume = 1.0f);
    void Stop(SoundID id);
    void StopAll();

    /// <summary>
    /// マスターボリュームの設定
    /// </summary>
    void SetMasterVolume(float volume);
    float GetMasterVolume() const { return m_MasterVolume; }

private:
    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    Audio m_Sounds[(int)SoundID::Count]{}; // SundIDごとのAudio
    
    // マスターボリューム
    float m_MasterVolume = 1.0f;

    // シングルトンインスタンス
    static SoundManager* s_Instance;
};