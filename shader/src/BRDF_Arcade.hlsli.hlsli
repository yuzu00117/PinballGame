#pragma once

#include "Common.hlsli"
#include "Material.hlsli"

float3 BRDF_Arcade(float3 N, float3 V, float3 L, float3 radiance, MaterialParams m)
{
    N = SafeNormalize(N);
    V = SafeNormalize(V);
    L = SafeNormalize(L);

    float NdotL = saturate(dot(N, L));
    float3 H = SafeNormalize(L + V);

    // ざっくりスペキュラ：roughness→shininess 変換（好みで調整）
    float shininess = lerp(128.0f, 8.0f, saturate(m.roughness));
    float spec = pow(saturate(dot(N, H)), shininess);

    float3 diffuse = m.baseColor * NdotL;
    float3 specular = lerp(0.04f.xxx, m.baseColor, saturate(m.metallic)) * spec;

    return (diffuse + specular) * radiance;
}
