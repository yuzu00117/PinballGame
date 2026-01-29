#pragma once

// �V�X�e���֘A�w�b�_
#include <DirectXMath.h>
#include <windows.h>
// �Q�[���֘A�w�b�_
#include "GameObject.h"
// �R���|�[�l���g�֘A�w�b�_
#include "Transform.h"

using namespace DirectX;

/// <summary>
/// �J�����N���X
/// </summary>
class Camera : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // �֐���`
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ���C�t�T�C�N�����\�b�h
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// �J�����ʒu��getter
    /// </summary>
    const XMFLOAT3& GetPosition() const { return m_Position; }

    /// <summary>
    /// �J�����̒����_��getter
    /// </summary>
    const XMFLOAT3& GetTarget() const { return m_Target; }
    
    /// <summary>
    /// �J�����̉�]�p��擾
    /// </summary>
    float GetYaw() const { return m_Yaw; }

    /// <summary>
    /// �r���[�s���getter
    /// </summary>
    const XMMATRIX& GetViewMatrix() const { return m_View; }

    /// <summary>
    /// Transform�ւ̃A�N�Z�X
    /// �J�����̈ʒu�E��]�E�X�P�[����O������G�肽���ꍇ�p
    /// </summary>
    Transform*       GetTransform()       { return &m_Transform; }
    const Transform* GetTransform() const { return &m_Transform; }
    
    /// <summary>
    /// �J������]����
    /// </summary>
    void RotateYaw(float angle) { m_Yaw += angle; }
    void RotatePitch(float angle);
    void SetDistance(float distance);

private:
    // ------------------------------------------------------------------------------
    // �萔��`
    // ------------------------------------------------------------------------------
    // �J�����̏����ݒ�l
    static constexpr float kDefaultYaw = XM_PI;                             // �f�t�H���g��Y�����̉�]�p�i���W�A���j
    static constexpr float kDefaultPitch = 0.7f;                            // �f�t�H���g��X�����̉�]�p�i���W�A���j
    static constexpr float kDefaultDistance = 8.0f;                         // �f�t�H���g�̃v���C���[����̋���
    static constexpr XMFLOAT3 kDefaultCameraOffset{ 0.0f, 25.0f, -15.0f };  // �f�t�H���g�̃J�����I�t�Z�b�g

    // ------------------------------------------------------------------------------
    // �ϐ���`
    // ------------------------------------------------------------------------------
    // �R���|�[�l���g
    Transform m_Transform;                                                  // Transform�R���|�[�l���g

    // �J�����s��֘A
    XMMATRIX m_Projection;                                                  // �v���W�F�N�V�����s��
    XMMATRIX m_View;                                                        // �r���[�s��

    // �֋X��L���b�V��
    XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };                                // �J�����ʒu
    XMFLOAT3 m_Target{ 0.0f, 0.0f, 0.0f };                                  // �J�����̒����_
    XMFLOAT3 m_CameraOffset = kDefaultCameraOffset;                         // �J�����̃I�t�Z�b�g
    
    // �J�����̉�]�p�Ƌ���
    float m_Yaw = kDefaultYaw;                                              // Y�����̉�]�p�i���W�A���j
    float m_Pitch = kDefaultPitch;                                          // X�����̉�]�p�i���W�A���j
    float m_Distance = kDefaultDistance;                                    // �v���C���[����̋���

    // �}�E�X����p
	bool    m_FirstMouse = true;                                            // ����}�E�X����t���O
	POINT   m_CenterPos;                                                    // �}�E�X�̒��S�ʒu
	float   m_MouseSensitivity = 0.002f;                                    // �}�E�X���x

    // �f�o�b�O�J�����p
    bool m_DebugCameraMode = false;                                         // �f�o�b�O�J�������[�h�t���O
    float m_DebugCameraSpeed = 0.15f;                                        // �f�o�b�O�J�����̈ړ����x
};
