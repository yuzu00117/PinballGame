#include "common.hlsli"

Texture2D      g_Texture      : register(t0);
SamplerState   g_SamplerState : register(s0);

cbuffer CBMaterial : register(b2)
{
    float4 gMatDiffuse;     // VSと同じCBを想定（未使用でも一致させる）
    int    gTextureEnable;  // 0/1
    float3 pad;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Diffuse  : COLOR0;
};

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    if (gTextureEnable != 0)
    {
        outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord) * In.Diffuse;
    }
    else
    {
        outDiffuse = In.Diffuse;
    }
}
