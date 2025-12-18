#pragma once

#include "Common.hlsli"
#include "Material.hlsli"

float ToonRamp(float x, float steps, float smoothWidth)
{
    // steps: 2..6 くらい想定
    steps = max(steps, 1.0f);
    float t = x * steps;
    float base = floor(t) / steps;
    float frac = t - floor(t);
    // 境界だけ少し滑らかに
    float s = smoothstep(0.5f - smoothWidth, 0.5f + smoothWidth, frac);
    return base + s * (1.0f / steps);
}

float3 BRDF_Toon(float3 N, float3 V, float3 L, float3 radiance, MaterialParams m)
{
    N = SafeNormalize(N);
    V = SafeNormalize(V);
    L = SafeNormalize(L);

    float NdotL = saturate(dot(N, L));

    float ramp = ToonRamp(NdotL, m.toonSteps, m.toonSmooth);
    float3 diffuse = m.baseColor * ramp;

    // トゥーンスペキュラ：ハイライトを段階化（必要なければ0にしてもいい）
    float3 H = SafeNormalize(L + V);
    float specRaw = pow(saturate(dot(N, H)), lerp(128.0f, 16.0f, m.roughness));
    float specToon = step(0.5f, specRaw); // 超シンプル（後で閾値パラメータ化推奨）
    float3 specular = 0.04f.xxx * specToon;

    return (diffuse + specular) * radiance;
}
