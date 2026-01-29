// soundManager.cpp
#include "main.h"
#include "SoundManager.h"

// �ÓI�����o�̒�`
SoundManager* SoundManager::s_Instance = nullptr;

// ����������
void SoundManager::Init()
{
    // �V���O���g���C���X�^���X�̐ݒ�
    s_Instance = this;

    // --- ���ʉ��̓ǂݍ��� ---
    // �����ŃT�E���h��ǂݍ���
}

// �j������
void SoundManager::Uninit()
{
    // �������C���X�^���X�Ȃ�J��
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }

    // �o�^�ς݃T�E���h�̉��
    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Uninit();
    }
}

// �X�V����
void SoundManager::Update(float deltaTime)
{
    // ���ɍX�V�����͂Ȃ��i�����Ǘ��Ȃ̂Łj
    // �����I�ɂ́A�t�F�[�h������ꊇ�~���[�g�Ȃǂ�����ōs������
}

// �`�揈��
void SoundManager::Draw()
{
    // �`�揈���͓��ɂȂ��i�����Ǘ��Ȃ̂Łj
}

// �T�E���h�̓ǂݍ���
void SoundManager::Load(SoundID id, const char* filename)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Load(filename);
}

// ------------------------------------------------------------------------------
// BGM�ESE�̍Đ� / ��~
// ------------------------------------------------------------------------------
// �T�E���h�̍Đ�
void SoundManager::Play(SoundID id, bool loop, float volume)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    float finalVolume = volume * m_MasterVolume;

    m_Sounds[index].SetVolume(finalVolume);
    m_Sounds[index].Play(loop);
}

// �T�E���h�̒�~
void SoundManager::Stop(SoundID id)
{
    int index = static_cast<int>(id);
    if (index < 0 || index >= (int)SoundID::Count) return;

    m_Sounds[index].Stop();
}

// ���ׂẴT�E���h���~
void SoundManager::StopAll()
{
    for (int i = 0; i < (int)SoundID::Count; ++i)
    {
        m_Sounds[i].Stop();
    }
}

// ------------------------------------------------------------------------------
// ���ʐݒ�
// ------------------------------------------------------------------------------
void SoundManager::SetMasterVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    m_MasterVolume = volume;
}