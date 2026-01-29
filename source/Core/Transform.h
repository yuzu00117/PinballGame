#pragma once

#include "vector3.h"
#include <DirectXMath.h>
using namespace DirectX;

/// <summary>
/// Transform�N���X
/// �I�u�W�F�N�g�̈ʒu�A��]�A�X�P�[����Ǘ�����
/// </summary>
struct Transform
{
    Vector3 Position = { 0.0f, 0.0f, 0.0f };    // �ʒu
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };    // ��]�i�x�j
    Vector3 Scale    = { 1.0f, 1.0f, 1.0f };    // �X�P�[��
    Transform* Parent = nullptr;                // �eTransform�ւ̃|�C���^

    /// <summary>
    /// ���[�J���s��𐶐����ĕԂ�
    /// </summary>
    XMMATRIX GetLocalMatrix() const
    {
        const auto ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
        const auto RotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(Rotation.x),
            XMConvertToRadians(Rotation.y),
            XMConvertToRadians(Rotation.z));
        const auto TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
        return ScaleMatrix * RotationMatrix * TranslationMatrix;
    }

    /// <summary>
    /// ���[���h�s��𐶐����ĕԂ�
    /// </summary>
    XMMATRIX GetWorldMatrix() const
    {
        const auto LocalMatrix = GetLocalMatrix();
        // �e������ꍇ�͐e�̃��[���h�s���|�����킹��
        return Parent ? (LocalMatrix * Parent->GetWorldMatrix()) : LocalMatrix;
    }

    /// <summary>
    /// �eTransform��ݒ肷��
    /// </summary>
    void SetParent(Transform* parent) { Parent = parent; }

    /// <summary>
    /// �eTransform��N���A����
    /// </summary>
    void ClearParent() { Parent = nullptr; }
};