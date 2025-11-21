#include "ColliderUtility.h"
#include <cmath>
#include <algorithm>

// ----------------------------------------------------------------------
// 線分と拡張AABBの衝突判定
// ----------------------------------------------------------------------
bool IntersectSegmentExpandedAABB(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& boxMin,
    const Vector3& boxMax,
    float          radius,
    CcdHit*        outHit)
{
    // AABBを玉の半径分だけ拡張する
    Vector3 bMin = boxMin - Vector3(radius, radius, radius);
    Vector3 bMax = boxMax + Vector3(radius, radius, radius);

    Vector3 dir = p1 - p0;
    const float eps = 1e-6f;

    float tMin = 0.0f;
    float tMax = 1.0f;

    Vector3 hitNormal(0, 0, 0);

    auto sweepAxis = [&](int axis, float boxMinAxis, float boxMaxAxis, float p0Axis, float dirAxis)
    {
        if (std::fabs(dirAxis) < eps)
        {
            // ほぼ平行（移動しない）場合: その軸でAABB範囲外なら衝突しない
            if (p0Axis < boxMinAxis || p0Axis > boxMaxAxis)
            {
                return false;
            }
            // 範囲内ならtMin, tMaxは変化しない
            return true;
        }

        float invDir = 1.0f / dirAxis;
        float t1 = (boxMinAxis - p0Axis) * invDir;
        float t2 = (boxMaxAxis - p0Axis) * invDir;
        float enter = t1;
        float exit = t2;

        // enter←exitの順にする
        Vector3 axisEnterNormal(0.0f, 0.0f, 0.0f);
        if (enter > exit)
        {
            std::swap(enter, exit);
            // 逆向きから入るときは法線も反転
        }

        // 法線は「入ってきた面」の外向き
        if (axis == 0)
        {
            axisEnterNormal = Vector3((invDir > 0.0f) ? -1.0f : 1.0f, 0.0f, 0.0f);
        }
        else if (axis == 1)
        {
            axisEnterNormal = Vector3(0.0f, (invDir > 0.0f) ? -1.0f : 1.0f, 0.0f);
        }
        else // axis == 2
        {
            axisEnterNormal = Vector3(0.0f, 0.0f, (invDir > 0.0f) ? -1.0f : 1.0f);
        }

        if (enter > tMin)
        {
            tMin = enter;
            hitNormal = axisEnterNormal;
        }
        if (exit < tMax)
        {
            tMax = exit;
        }

        // tの範囲が食い違ったら衝突しない
        if (tMin > tMax)
        {
            return false;
        }
        return true;
    };

    // 各軸でスイープテスト
    // X軸
    if (!sweepAxis(0, bMin.x, bMax.x, p0.x, dir.x)) return false;
    // Y軸
    if (!sweepAxis(1, bMin.y, bMax.y, p0.y, dir.y)) return false;
    // Z軸
    if (!sweepAxis(2, bMin.z, bMax.z, p0.z, dir.z)) return false;

    // 線分の範囲外でのみ重なる場合は衝突しない
    if (tMax < 0.0f || tMin > 1.0f)
    {
        return false;
    }

    // 実際に使うtは[0,1]にクリップ
    float tHit = std::max(tMin, 0.0f);

    if (outHit)
    {
        outHit->t = tHit;
        outHit->point = p0 + dir * tHit;
        outHit->normal = hitNormal;
    }

    return true;
}