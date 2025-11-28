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
    Vector3 Position = { 0.0f, 0.0f, 0.0f };    // 位置
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };    // 回転（度）
    Vector3 Scale    = { 1.0f, 1.0f, 1.0f };    // スケール
    Transform* Parent = nullptr;                // 親Transformへのポインタ

    /// <summary>
    /// ローカル行列を生成して返す
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
    /// ワールド行列を生成して返す
    /// </summary>
    XMMATRIX GetWorldMatrix() const
    {
        const auto LocalMatrix = GetLocalMatrix();
        // 親がいる場合は親のワールド行列を掛け合わせる
        return Parent ? (LocalMatrix * Parent->GetWorldMatrix()) : LocalMatrix;
    }

    /// <summary>
    /// 親Transformを設定する
    /// </summary>
    void SetParent(Transform* parent) { Parent = parent; }

    /// <summary>
    /// 親Transformをクリアする
    /// </summary>
    void ClearParent() { Parent = nullptr; }
};