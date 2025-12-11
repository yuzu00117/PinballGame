#pragma once

#include "gameObject.h"
#include "audio.h"
#include "SoundID.h"

/// <summary>
/// サウンド管理クラス
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
    void Update() override;
    void Draw() override;

    /// <summary>
    /// BGM・SEの再生
    /// オーバーロードで分けているので、受け取ったIDに応じて適切なサウンドを再生する
    /// </summary>
    void Play(BGMID id, bool loop = true, float volume = 1.0f);
    void Play(SEID id, bool loop = false, float volume = 1.0f);

    /// <summary>
    /// 音量設定
    /// </summary>
    void SetBGMVolume(float volume);
    void SetSEVolume(float volume);

private:
    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    // BGM・SEをそれぞれIDごとに保持
    Audio m_BGMs[(int)BGMID::Count]{};
    Audio m_SEs[(int)SEID::Count]{};
    
    // グループ音量
    float m_BGMVolume = 1.0f;
    float m_SEVolume = 1.0f;

    // シングルトンインスタンス
    static SoundManager* s_Instance;
};