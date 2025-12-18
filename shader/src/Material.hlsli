#pragma once

struct MaterialParams
{
    float3 baseColor;   // 定数色（テクスチャがあるなら乗算）
    float  roughness;   // 0..1
    float  metallic;    // 0..1
    float3 emissive;    // 発光（任意）
    float  rimPower;    // リムの鋭さ（大きいほど細い）
    float3 rimColor;    // リム色
    float  rimIntensity;// リム強さ
    float  toonSteps;   // トゥーン段数（例: 3,4）
    float  toonSmooth;  // 境界スムーズ（例: 0.02）
};

// マスク1枚にパックするなら：R=Roughness, G=Metallic, B=AO, A=EmissiveMask など
struct MaterialMasks
{
    float roughness;
    float metallic;
    float ao;
    float emissiveMask;
};
