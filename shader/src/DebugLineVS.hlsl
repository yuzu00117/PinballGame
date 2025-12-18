#include "common.hlsli"

cbuffer CBObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProj;
};

struct VS_IN_LINE  { float3 Position : POSITION; float4 Color : COLOR; };
struct PS_IN_LINE  { float4 Position : SV_Position; float4 Color : COLOR; };

void main(in VS_IN_LINE In, out PS_IN_LINE Out)
{
    float4x4 wvp = mul(gWorld, gView);
    wvp = mul(wvp, gProj);

    Out.Position = mul(float4(In.Position, 1.0f), wvp);
    Out.Color    = In.Color;
}
