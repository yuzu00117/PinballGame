#pragma once

#include <memory>

class GameObject; // �O���錾

/// <summary>
/// ���ׂẴR���|�[�l���g�̊��N���X
/// </summary>
class Component
{
public:
	// --- �֐���` ---
	// �f�t�H���g�R���X�g���N�^�E�f�X�g���N�^
	Component() = default;
	virtual ~Component() = default;
	
	/// <summary>
	/// ���C�t�T�C�N�����\�b�h
	/// </summary>
	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update(float deltaTime) {};
	virtual void Draw() {};

	// --- �ϐ���` ---
	GameObject* m_Owner = nullptr; // ��������I�u�W�F�N�g�ւ̃|�C���^
};