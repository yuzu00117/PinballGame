#include "common.hlsli"

struct VS_IN_LINE  { float3 Position : POSITION; float4 Color : COLOR; };
struct PS_IN_LINE  { float4 Position : SV_Position; float4 Color : COLOR; };

void main(in VS_IN_LINE In, out PS_IN_LINE Out)
{
    // 既存の unlit と同じ合成順（World→View→Projection）
    // ※ BoxCollider 側で既に World を掛けて座標を渡しているなら、
    //    World を Identity にしておけば二重変換にはなりません。
    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);

    Out.Position = mul(float4(In.Position, 1.0), wvp);
    Out.Color    = In.Color;
}
