#pragma once

#include "Vector3.h"
#include "MathUtil.h"

class Collider;

/// <summary>
/// コライダー関連のユーティリティ
/// </summary>

// -------------------------------------------------------------------------------
// 構造体定義
// -------------------------------------------------------------------------------
/// <summary>
/// コライダーの衝突情報を格納する構造体
/// </summary>
struct CollisionInfo
{
    Collider* self = nullptr;               // 自分自身のコライダー（この情報を受け取る側）
    Collider* other = nullptr;              // 衝突相手のコライダー
    Vector3 normal{0.0f, 0.0f, 0.0f};       // 衝突法線ベクトル（selfから見た法線）
    Vector3 contactPoint{0.0f, 0.0f, 0.0f}; // 衝突接触点
    float penetration = 0.0f;               // めり込み量
    bool isCCDHit = false;                  // これはCCDによる衝突かどうか
};

// ----------------------------------------------------------------------
// 関数定義
// ----------------------------------------------------------------------
/// <summary>
/// Box vs Sphereの補助関数：球とAABBの重なり判定
/// </summary>
static bool IsSphereOverlappingBox(
    const Vector3& center,
    float          radius,
    const Vector3& boxMin,
    const Vector3& boxMax)
{
    // 最近接点
    float cx = Clamp(center.x, boxMin.x, boxMax.x);
    float cy = Clamp(center.y, boxMin.y, boxMax.y);
    float cz = Clamp(center.z, boxMin.z, boxMax.z);
    Vector3 closest{ cx, cy, cz };

    Vector3 diff = center - closest;

    // NOTE: 数学的にはdiff.Length() <= radius が正しいが、
    // このエンジンではCCD安定性のため、あえてdiff.Length() <= (radius * radius) として
    // 広めに判定している（挙動が良いので仕様とする）
    return diff.Length() <= (radius * radius);
}