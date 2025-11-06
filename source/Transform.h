#pragma once

#include "vector3.h"
#include <DirectXMath.h>
using namespace DirectX;

/// <summary>
/// Transformクラス
/// オブジェクトの位置、回転、スケールを管理する
/// </summary>
struct Transform
{
    Vector3 Position = { 0.0f, 0.0f, 0.0f }; // 位置
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f }; // 回転（オイラー角）
    Vector3 Scale    = { 1.0f, 1.0f, 1.0f }; // スケール

    /// <summary>
    /// ワールド行列を生成して返す
    /// </summary>
    XMMATRIX GetWorldMatrix() const
    {
        return XMMatrixScaling(Scale.x, Scale.y, Scale.z)
            * XMMatrixRotationRollPitchYaw(
                XMConvertToRadians(Rotation.x),
                XMConvertToRadians(Rotation.y),
                XMConvertToRadians(Rotation.z))
            * XMMatrixTranslation(Position.x, Position.y, Position.z);
    }
};