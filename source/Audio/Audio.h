#pragma once

#include <xaudio2.h>

/// <summary>
/// �I�[�f�B�I�Ǘ��N���X
/// </summary>
class Audio
{
public:
	// ------------------------------------------------------------------------------
	// �֐���`
	// ------------------------------------------------------------------------------
	/// <summary>
	/// �I�[�f�B�I�V�X�e���̏������E�j������
	/// </summary>
	static void InitMaster();
	static void UninitMaster();

	/// <summary>
	/// �I�[�f�B�I�R���|�[�l���g�̔j������
	/// </summary>
	void Uninit();

	/// <summary>
	/// �T�E���h�f�[�^�̓ǂݍ���
	/// </summary>
	void Load(const char *FileName);

	///	<summary>
	/// �T�E���h�̍Đ��E��~
	/// </summary>
	void Play(bool Loop = false);
	void Stop();

	///	<summary>
	/// ���ʐݒ�
	/// </summary>
	void SetVolume(float Volume);

	/// <summary>
	/// �������ς݂��ǂ����擾
	/// </summary>
	bool IsInitialized() const { return m_Initialized; }

private:
	// ------------------------------------------------------------------------------
	// �ϐ���`
	// ------------------------------------------------------------------------------
	static IXAudio2*				m_Xaudio;			   // XAudio�C���^�[�t�F�[�X
	static IXAudio2MasteringVoice*	m_MasteringVoice;	   // �}�X�^�����O�{�C�X

	IXAudio2SourceVoice*			m_SourceVoice{};	   // �\�[�X�{�C�X
	BYTE*							m_SoundData{};		   // �T�E���h�f�[�^

	int								m_Length{};			   // �T�E���h�f�[�^��
	int								m_PlayLength{};		   // �Đ���
	bool 							m_Initialized = false; // �������ς݃t���O
};