#pragma once

#include "Common.hlsli"

struct DirectionalLight
{
    float3 directionWS; // 「光が進む方向」(例: 太陽光が下に向かうなら 0,-1,0)
    float  intensity;
    float3 color;
    float  pad0;
};

struct PointLight
{
    float3 positionWS;
    float  range;       // 有効距離
    float3 color;
    float  intensity;
};

struct SpotLight
{
    float3 positionWS;
    float  range;
    float3 directionWS; // 「光が進む方向」
    float  innerCos;    // cos(innerAngle)
    float  outerCos;    // cos(outerAngle)
    float3 color;
    float  intensity;
};

// 減衰（シンプルに扱いやすい形）
float AttenuationDistance(float dist, float range)
{
    // range外は0
    float x = saturate(1.0f - dist / max(range, 1e-4f));
    // それっぽい落ち方（調整しやすい）
    return x * x;
}

float AttenuationSpot(float3 LdirWS, float3 spotDirWS, float innerCos, float outerCos)
{
    // spotDirWSは「光が進む方向」。LdirWSは「点→光」ではなく「点→ライト」等で符号が変わるので統一する
    // ここでは LtoP = normalize(P - lightPos) を使う前提で、光軸と同方向を + にする
    float cd = dot(SafeNormalize(LdirWS), SafeNormalize(spotDirWS));
    return smoothstep(outerCos, innerCos, cd);
}

struct LightSample
{
    float3 L;        // 点からライト方向（正規化）
    float3 radiance; // 色*強度*減衰
};

// Directional: L は「点→ライト」ではなく「点→光源方向」(= -directionWS) として扱うと混乱が減る
LightSample SampleDirectional(DirectionalLight lt)
{
    LightSample s;
    s.L = SafeNormalize(-lt.directionWS);
    s.radiance = lt.color * lt.intensity;
    return s;
}

LightSample SamplePoint(PointLight lt, float3 Pws)
{
    LightSample s;
    float3 toL = lt.positionWS - Pws;
    float dist = length(toL);
    s.L = (dist > 1e-4f) ? (toL / dist) : float3(0,1,0);
    float att = AttenuationDistance(dist, lt.range);
    s.radiance = lt.color * lt.intensity * att;
    return s;
}

LightSample SampleSpot(SpotLight lt, float3 Pws)
{
    LightSample s;
    float3 toL = lt.positionWS - Pws;
    float dist = length(toL);
    s.L = (dist > 1e-4f) ? (toL / dist) : float3(0,1,0);

    float attD = AttenuationDistance(dist, lt.range);

    // スポット角：光軸と、ライト→点方向で評価するのが一般的
    float3 lightToPointDir = SafeNormalize(Pws - lt.positionWS);
    float attS = AttenuationSpot(lightToPointDir, SafeNormalize(lt.directionWS), lt.innerCos, lt.outerCos);

    s.radiance = lt.color * lt.intensity * attD * attS;
    return s;
}
