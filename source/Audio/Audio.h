#pragma once

#include <xaudio2.h>
#include "GameObject.h"
#include "Component.h"

/// <summary>
/// オーディオ管理クラス
/// </summary>
class Audio : public Component
{
public:
	// ------------------------------------------------------------------------------
	// 型定義
	// ------------------------------------------------------------------------------
	/// <summary>
	/// 基底クラスComponentのコンストラクタを継承
	/// </summary>
	using Component::Component;
	// ------------------------------------------------------------------------------
	// 関数定義
	// ------------------------------------------------------------------------------
	/// <summary>
	/// オーディオシステムの初期化・破棄処理
	/// </summary>
	static void InitMaster();
	static void UninitMaster();

	/// <summary>
	/// オーディオコンポーネントの破棄処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// サウンドデータの読み込み
	/// </summary>
	void Load(const char *FileName);

	///	<summary>
	/// サウンドの再生
	/// </summary>
	void Play(bool Loop = false);

	///	<summary>
	/// 音量設定
	/// </summary>
	void SetVolume(float Volume);

private:
	// ------------------------------------------------------------------------------
	// 変数定義
	// ------------------------------------------------------------------------------
	static IXAudio2*				m_Xaudio;			   // XAudioインターフェース
	static IXAudio2MasteringVoice*	m_MasteringVoice;	   // マスタリングボイス

	IXAudio2SourceVoice*			m_SourceVoice{};	   // ソースボイス
	BYTE*							m_SoundData{};		   // サウンドデータ

	int								m_Length{};			   // サウンドデータ長
	int								m_PlayLength{};		   // 再生長

	bool 							m_Initialized = false; // 初期化済みフラグ
};