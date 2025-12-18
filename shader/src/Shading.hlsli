#pragma once

#include "Lighting.hlsli"
#include "Material.hlsli"

// どちらかを include する（PS側で切替）
float3 ApplyRim(float3 color, float3 N, float3 V, MaterialParams m)
{
    float rim = 1.0f - saturate(dot(SafeNormalize(N), SafeNormalize(V)));
    rim = pow(rim, max(m.rimPower, 1e-3f)) * m.rimIntensity;
    return color + m.rimColor * rim;
}
