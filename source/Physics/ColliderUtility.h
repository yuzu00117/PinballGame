#pragma once

#include "Vector3.h"

struct CcdHit
{
    float t;        // 始点→終点の補完係数
    Vector3 point;  // 衝突位置
    Vector3 normal; // 衝突法線
};

bool IntersectSegmentExpandedAABB(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& boxMin,
    const Vector3& boxMax,
    float          radius,
    CcdHit*        outHit);