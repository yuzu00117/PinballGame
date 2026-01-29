//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------
// 役割:
// シーンノードの位置・回転・スケールを保持し、
// ローカル行列およびワールド行列を生成するためのデータ構造。
//
// 設計意図:
// GameObject や Component から参照される「空間情報」を集約する。
// 親 Transform を持つことで階層構造（親子関係）を表現し、
// 親の変換を含んだワールド行列を再帰的に計算できるようにしている。
//
// 構成:
// - Position               : 位置（ワールド／ローカル解釈は利用側に委ねる）
// - Rotation               : 回転（度数法、XYZ＝Pitch/Yaw/Roll）
// - Scale                  : スケール
// - Parent                 : 親 Transform への参照（非所有）
// - 行列生成               : Local / World 行列生成関数
//
// NOTE:
// - 回転は度数法で保持し、行列生成時にラジアンへ変換する
// - Parent は非所有参照のため、親の寿命管理は外部（GameObject）が責任を持つ
// - 循環参照（Parent が自分自身や子孫を指す状態）は想定していない
//------------------------------------------------------------------------------
#pragma once

#include "vector3.h"
#include <DirectXMath.h>

using namespace DirectX;

/// Transform 情報を表す構造体
/// - 位置・回転・スケールを保持する
/// - 親 Transform を持つことで階層的な変換を表現できる
struct Transform
{
    // ----------------------------------------------------------------------
    // Transformデータ
    // ----------------------------------------------------------------------
    Vector3 Position = { 0.0f, 0.0f, 0.0f };   // 位置
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };   // 回転（度数法）
    Vector3 Scale    = { 1.0f, 1.0f, 1.0f };   // スケール

    Transform* Parent = nullptr;               // 親Transform（非所有）

    // ----------------------------------------------------------------------
    // 行列生成
    // ----------------------------------------------------------------------
    /// ローカル行列を生成する
    /// - 計算順：Scale → Rotation → Translation
    /// - Rotation は度数法からラジアンに変換される
    XMMATRIX GetLocalMatrix() const
    {
        const auto scaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
        const auto rotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(Rotation.x),
            XMConvertToRadians(Rotation.y),
            XMConvertToRadians(Rotation.z));
        const auto translationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    /// ワールド行列を生成する
    /// - Parent が存在する場合、親の World 行列を再帰的に合成する
    /// - Parent が nullptr の場合は Local 行列をそのまま返す
    XMMATRIX GetWorldMatrix() const
    {
        const auto localMatrix = GetLocalMatrix();
        return Parent ? (localMatrix * Parent->GetWorldMatrix()) : localMatrix;
    }

    // ----------------------------------------------------------------------
    // 親子関係管理
    // ----------------------------------------------------------------------
    /// 親 Transform を設定する
    /// - 所有権は移動しない（非所有参照）
    /// NOTE: 親の寿命は呼び出し側で保証すること
    void SetParent(Transform* parent) { Parent = parent; }

    /// 親 Transform を解除する
    void ClearParent() { Parent = nullptr; }
};
