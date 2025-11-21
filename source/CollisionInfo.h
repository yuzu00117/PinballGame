#pragma once

#include "Vector3.h"

class Collider;

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