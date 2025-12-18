// BaseLitVS.hlsl
// 目的：posWS / normalWS / uv をPSへ渡す（NormalMap無し）

cbuffer CBObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProj;
};

struct VSIn
{
    float3 posL : POSITION;
    float3 nrmL : NORMAL;
    float2 uv   : TEXCOORD0;
};

struct VSOut
{
    float4 posH   : SV_POSITION;
    float3 posWS  : TEXCOORD0;
    float3 nrmWS  : TEXCOORD1;
    float2 uv     : TEXCOORD2;
};

VSOut main(VSIn v)
{
    VSOut o;

    float4 posW = mul(float4(v.posL, 1.0f), gWorld);
    o.posWS = posW.xyz;

    // 法線は w=0 でWorldへ（※非一様スケールがあるなら逆転置が必要）
    o.nrmWS = mul(float4(v.nrmL, 0.0f), gWorld).xyz;

    float4 posV = mul(posW, gView);
    o.posH = mul(posV, gProj);

    o.uv = v.uv;
    return o;
}
