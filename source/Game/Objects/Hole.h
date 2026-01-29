#pragma once

#include "gameobject.h"
#include "vector3.h"

// �O���錾
class MeshRenderer;
class ColliderGroup;

/// <summary>
/// �s���{�[���̃z�[���i���j�I�u�W�F�N�g
/// �R���C�_�[�ɓ��������Ƃ��ɁABall / Enemy�𔻒肵�ď�������
/// </summary>
class Hole : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    //  �֐���` 
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ���C�t�T�C�N�����\�b�h
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// �Փ˃R�[���o�b�N
    /// Hole�̃R���C�_�[�ɓ��������Ƃ��̏���
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// <summary>
    /// �z�[���ʒu��Getter
    /// </summary>
    Vector3 GetHolePosition() const { return m_Transform.Position; }

private:
    // ------------------------------------------------------------------------------
    // �萔��`
    // ------------------------------------------------------------------------------
    static constexpr const char* VertexShaderPath = // ���_�V�F�[�_�̃p�X
        "shader\\bin\\BaseLitVS.cso";   
    static constexpr const char* PixelShaderPath  = // �s�N�Z���V�F�[�_�̃p�X
        "shader\\bin\\BaseLitPS.cso";   

    // ------------------------------------------------------------------------------
    // �ϐ���`
    // ------------------------------------------------------------------------------
    // �����ځE�����蔻��p�R���|�[�l���g
    MeshRenderer* m_MeshRenderer = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
};
