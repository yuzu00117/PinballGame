#pragma once

#include "vector3.h"
#include <DirectXMath.h>
using namespace DirectX;

/// <summary>
/// </summary>
struct Transform
{
    Vector3 Position = { 0.0f, 0.0f, 0.0f };
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 Scale    = { 1.0f, 1.0f, 1.0f };
    Transform* Parent = nullptr;

    /// <summary>
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
    /// </summary>
    XMMATRIX GetWorldMatrix() const
    {
        const auto LocalMatrix = GetLocalMatrix();
        return Parent ? (LocalMatrix * Parent->GetWorldMatrix()) : LocalMatrix;
    }

    /// <summary>
    /// </summary>
    void SetParent(Transform* parent) { Parent = parent; }

    /// <summary>
    /// </summary>
    void ClearParent() { Parent = nullptr; }
};
