// DebugLineVS.hlsl
#include "Common.hlsli"

cbuffer CBWorld : register(b0)
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose; // 使わないが、b0のサイズ一致のため入れておく
};

cbuffer CBView : register(b1)
{
    float4x4 gView;
};

cbuffer CBProjection : register(b2)
{
    float4x4 gProj;
};

struct VS_IN_LINE
{
    float3 Position : POSITION;
    float4 Color    : COLOR;
};

struct PS_IN_LINE
{
    float4 Position : SV_Position;
    float4 Color    : COLOR;
};

void main(in VS_IN_LINE In, out PS_IN_LINE Out)
{
    // row-vector style: v * W * V * P
    float4 posW = mul(float4(In.Position, 1.0f), gWorld);
    float4 posV = mul(posW, gView);
    Out.Position = mul(posV, gProj);

    Out.Color = In.Color;
}
