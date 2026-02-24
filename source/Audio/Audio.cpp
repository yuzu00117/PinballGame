#include "Audio.h"
#include "main.h"

// ------------------------------------------------------------------------------
// 静的メンバの初期化
// ------------------------------------------------------------------------------
// NOTE: アプリケーション起動時は nullptr。InitMaster で生成し UninitMaster で解放する
IXAudio2*               Audio::m_Xaudio         = nullptr;
IXAudio2MasteringVoice* Audio::m_MasteringVoice = nullptr;

// ------------------------------------------------------------------------------
// Master 初期化
// ------------------------------------------------------------------------------
// - COM を初期化し、XAudio2 インスタンスとマスタリングボイスを生成する
// NOTE: アプリケーション起動時に1回だけ呼ぶこと（多重呼び出し非対応）
void Audio::InitMaster()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    XAudio2Create(&m_Xaudio, 0);
    m_Xaudio->CreateMasteringVoice(&m_MasteringVoice);
}

// ------------------------------------------------------------------------------
// Master 終了
// ------------------------------------------------------------------------------
// - マスタリングボイスを破棄し、XAudio2 を解放、COM を終了する
// NOTE: すべての Audio インスタンスを Uninit してから呼ぶこと
void Audio::UninitMaster()
{
    m_MasteringVoice->DestroyVoice();
    m_Xaudio->Release();
    CoUninitialize();
}

// ------------------------------------------------------------------------------
// インスタンス終了
// ------------------------------------------------------------------------------
// - SourceVoice の停止 / 破棄と、音声バッファ（delete[]）の解放を行う
// - 未初期化の場合は何もしない
void Audio::Uninit()
{
    if (!m_Initialized)
    {
        return;
    }

    m_SourceVoice->Stop();
    m_SourceVoice->DestroyVoice();
    m_SourceVoice = nullptr;

    delete[] m_SoundData;
    m_SoundData = nullptr;

    m_Initialized = false;
}

// ------------------------------------------------------------------------------
// 音量設定
// ------------------------------------------------------------------------------
// - XAudio2 の SourceVoice に volume をそのまま渡す（範囲チェックなし）
// NOTE: Load 前に呼ぶと m_SourceVoice が null でクラッシュする
void Audio::SetVolume(float volume)
{
    assert(m_SourceVoice);
    m_SourceVoice->SetVolume(volume);
}

// ------------------------------------------------------------------------------
// 音声ロード
// ------------------------------------------------------------------------------
// - RIFF WAVE ファイルを読み込み、SourceVoice を生成する
// - 成功時：m_Initialized を true にする
// NOTE: 再ロードする場合は事前に Uninit を呼ぶこと（リーク防止）
void Audio::Load(const char* fileName)
{
    WAVEFORMATEX wfx = {};

    {
        HMMIO    hmmio         = nullptr;
        MMIOINFO mmioinfo      = {};
        MMCKINFO riffChunkInfo = {};
        MMCKINFO dataChunkInfo = {};
        MMCKINFO mmckInfo      = {};

        hmmio = mmioOpen(const_cast<LPSTR>(fileName), &mmioinfo, MMIO_READ);
        assert(hmmio);

        riffChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        mmioDescend(hmmio, &riffChunkInfo, nullptr, MMIO_FINDRIFF);

        mmckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
        mmioDescend(hmmio, &mmckInfo, &riffChunkInfo, MMIO_FINDCHUNK);

        if (mmckInfo.cksize >= sizeof(WAVEFORMATEX))
        {
            mmioRead(hmmio, reinterpret_cast<HPSTR>(&wfx), sizeof(wfx));
        }
        else
        {
            PCMWAVEFORMAT pcmwf = {};
            mmioRead(hmmio, reinterpret_cast<HPSTR>(&pcmwf), sizeof(pcmwf));
            memcpy(&wfx, &pcmwf, sizeof(pcmwf));
            wfx.cbSize = 0;
        }
        mmioAscend(hmmio, &mmckInfo, 0);

        dataChunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
        mmioDescend(hmmio, &dataChunkInfo, &riffChunkInfo, MMIO_FINDCHUNK);

        const UINT32 bufLen = dataChunkInfo.cksize;
        m_SoundData         = new BYTE[bufLen];
        const LONG readLen  = mmioRead(hmmio, reinterpret_cast<HPSTR>(m_SoundData), bufLen);

        m_Length     = readLen;
        m_PlayLength = readLen / wfx.nBlockAlign;

        mmioClose(hmmio, 0);
    }

    m_Xaudio->CreateSourceVoice(&m_SourceVoice, &wfx);
    assert(m_SourceVoice);

    m_Initialized = true;
}

// ------------------------------------------------------------------------------
// 再生
// ------------------------------------------------------------------------------
// - バッファを設定して SourceVoice を再生する
// - loop = true の場合、無限ループ再生する
// NOTE: Load 前に呼ぶと m_SourceVoice が null でクラッシュする
void Audio::Play(bool loop)
{
    m_SourceVoice->Stop();
    m_SourceVoice->FlushSourceBuffers();

    XAUDIO2_BUFFER bufInfo = {};
    bufInfo.AudioBytes = m_Length;
    bufInfo.pAudioData = m_SoundData;
    bufInfo.PlayBegin  = 0;
    bufInfo.PlayLength = m_PlayLength;

    if (loop)
    {
        bufInfo.LoopBegin  = 0;
        bufInfo.LoopLength = m_PlayLength;
        bufInfo.LoopCount  = XAUDIO2_LOOP_INFINITE;
    }

    m_SourceVoice->SubmitSourceBuffer(&bufInfo, nullptr);
    m_SourceVoice->Start();
}

// ------------------------------------------------------------------------------
// 停止
// ------------------------------------------------------------------------------
// - SourceVoice を停止し、バッファをフラッシュする
// - 未初期化の場合は何もしない
void Audio::Stop()
{
    if (!m_Initialized)
    {
        return;
    }

    m_SourceVoice->Stop();
    m_SourceVoice->FlushSourceBuffers();
}