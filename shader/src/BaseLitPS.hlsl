// BaseLitPS.hlsl
// エンジン側（Renderer.cpp）の前提:
//  - PS 用 CB スロット: b3(Material), b4(Light)
// LIGHT は平行光源（Directional）のみ。

#include "Common.hlsli"

Texture2D gBaseMap : register(t0);
SamplerState gSampLinear : register(s0);

struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float  Shininess;
    int    TextureEnable;
    float2 Dummy;
};

struct LIGHT
{
    int    Enable;
    int3   Dummy;
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer CBMaterial : register(b3) { MATERIAL gMat; }
cbuffer CBLight    : register(b4) { LIGHT    gLight; }

struct PSIn
{
    float4 posH  : SV_POSITION;
    float3 posWS : TEXCOORD0;
    float3 nrmWS : TEXCOORD1;
    float4 col   : TEXCOORD2;
    float2 uv    : TEXCOORD3;
};

float4 main(PSIn i) : SV_TARGET
{
    float3 N = SafeNormalize(i.nrmWS);

    float3 L = SafeNormalize(-gLight.Direction.xyz);
    float ndl = saturate(dot(N, L));

    float3 baseColor = gMat.Diffuse.rgb * i.col.rgb;
    float alpha = gMat.Diffuse.a * i.col.a;

    // テクスチャ色を取得
    if (gMat.TextureEnable != 0)
    {
        float4 tex = gBaseMap.Sample(gSampLinear, i.uv);
        baseColor *= tex.rgb;
        alpha *= tex.a;
    }

    // ライティング無効時は環境光のみ返す
    if (gLight.Enable == 0)
    {
        return float4(baseColor * gMat.Ambient.rgb, alpha);
    }

    float3 ambient = gMat.Ambient.rgb * gLight.Ambient.rgb;
    float3 diffuse = baseColor * (gLight.Diffuse.rgb * ndl);

    float3 col = ambient + diffuse;

    return float4(col, alpha);
}
