//------------------------------------------------------------------------------
// Audio
//------------------------------------------------------------------------------
// 役割:
// XAudio2 を用いて音声（効果音/SE想定）のロードと再生を行う。
// Master（IXAudio2 / MasteringVoice）は静的に1つだけ初期化して共有する。
//
// 設計意図:
// XAudio2 のデバイス/マスター管理をクラス外へ漏らさず、
// 各Audioインスタンスは「音データ＋SourceVoice」の管理に集中させる。
//
// 構成:
// - Master管理（static） : InitMaster / UninitMaster, IXAudio2, MasteringVoice
// - インスタンス管理    : Load / Play / Stop / SetVolume / Uninit
//
// NOTE:
// - Load は RIFF WAVE（fmt/data チャンク）読み込みのみ対応。失敗時は assert で停止し、
//   失敗を呼び出し側へ返す仕組みはない。確保したバッファは Uninit で delete[] 解放する。
// - COM 初期化は InitMaster 内で CoInitializeEx(COINIT_MULTITHREADED) を実行し、
//   UninitMaster で CoUninitialize を呼ぶ。別箇所で COM を管理している場合は衝突に注意。
//------------------------------------------------------------------------------
#pragma once
#include <xaudio2.h>

/// 音声再生クラス（XAudio2）
/// - Masterは静的に共有し、インスタンスは SourceVoice / 音データを管理する
/// - 使い方: InitMaster→Load→Play/SetVolume/Stop、終了時に Uninit→UninitMaster
class Audio
{
public:
    // --------------------------------------------------------------------------
    // Master 管理（静的共有）
    // --------------------------------------------------------------------------
    /// XAudio2のマスターを初期化する（1回のみ呼ぶ）
    /// - 副作用：CoInitializeEx(COINIT_MULTITHREADED), IXAudio2 / MasteringVoice を生成
    /// - 事前条件：InitMaster は多重呼び出しを想定していない
    static void InitMaster();

    /// XAudio2のマスターを終了する
    /// - 副作用：MasteringVoice を DestroyVoice、IXAudio2 を Release、CoUninitialize を呼ぶ
    /// - 事前条件：InitMaster 成功後に対応する回数だけ呼ぶ
    static void UninitMaster();

    // --------------------------------------------------------------------------
    // インスタンス管理
    // --------------------------------------------------------------------------
    /// インスタンス側のリソースを解放する
    /// - 解放対象：SourceVoice の DestroyVoice / 音声バッファ（delete[]）
    /// - 未初期化の場合は何もしない
    void Uninit();

    /// 音声をロードする
    /// - 引数：ファイルパス（null終端文字列）
    /// - 対応形式：RIFF WAVE のみ（fmt/data チャンク）
    /// - 失敗時：assert で停止（戻り値や例外はなし）
    /// - 注意：再ロードする場合は事前に Uninit を呼ぶ（リーク防止）
    void Load(const char* fileName);

    /// 再生する
    /// - Loop=true の場合、ループ再生する
    /// - 事前条件：Load 済み（未ロードだと m_SourceVoice が null でクラッシュする）
    void Play(bool loop = false);

    /// 停止する
    void Stop();

    /// 音量を設定する
    /// - Volume の範囲チェックは行わない（XAudio2 にそのまま渡す）
    void SetVolume(float volume);

    /// 初期化済み（ロード済み）かどうか
    bool IsInitialized() const { return m_Initialized; }

private:
    // --------------------------------------------------------------------------
    // Master（静的共有）
    // --------------------------------------------------------------------------
    static IXAudio2*                m_Xaudio;
    static IXAudio2MasteringVoice*  m_MasteringVoice;

    // --------------------------------------------------------------------------
    // インスタンスリソース
    // --------------------------------------------------------------------------
    IXAudio2SourceVoice*            m_SourceVoice{};
    BYTE*                           m_SoundData{};

    int                             m_Length{};
    int                             m_PlayLength{};
    bool                            m_Initialized = false;
};


