#include "common.hlsli"

cbuffer CBObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProj;
};

cbuffer CBMaterial : register(b2)
{
    float4 gMatDiffuse;     // RGBA
    int    gTextureEnable;  // 0/1
    float3 pad;
};

struct VS_IN
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Diffuse  : COLOR0;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Diffuse  : COLOR0;
};

void main(in VS_IN In, out PS_IN Out)
{
    float4 posW = mul(float4(In.Position, 1.0f), gWorld);
    float4 posV = mul(posW, gView);
    Out.Position = mul(posV, gProj);

    Out.TexCoord = In.TexCoord;
    Out.Diffuse  = In.Diffuse * gMatDiffuse;
}
