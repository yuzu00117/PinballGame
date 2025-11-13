#include "common.hlsl"

struct PS_IN_LINE { float4 Position : SV_Position; float4 Color : COLOR; };

void main(in PS_IN_LINE In, out float4 outColor : SV_Target)
{
    outColor = In.Color;
}
