// BaseLitVS.hlsl
// エンジン側（Renderer.cpp）の前提:
//  - InputLayout: POSITION, NORMAL, COLOR, TEXCOORD
//  - VS 用 CB スロット: b0(World), b1(View), b2(Proj)
// 行列は Renderer::SetWorld/View/ProjectionMatrix で転置（TRANSPOSE）してアップロードされるため、
// 行ベクトル形式（row-vector style）として mul(v, M) を使用する。

cbuffer CBWorld      : register(b0)
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
};

cbuffer CBView       : register(b1) { float4x4 gView; }
cbuffer CBProjection : register(b2) { float4x4 gProj; }

struct VSIn
{
    float3 posL : POSITION;
    float3 nrmL : NORMAL;
    float4 col  : COLOR;
    float2 uv   : TEXCOORD0;
};

struct VSOut
{
    float4 posH  : SV_POSITION;
    float3 posWS : TEXCOORD0;
    float3 nrmWS : TEXCOORD1;
    float4 col   : TEXCOORD2;
    float2 uv    : TEXCOORD3;
};

VSOut main(VSIn v)
{
    VSOut o;

    float4 posW = mul(float4(v.posL, 1.0f), gWorld);
    o.posWS = posW.xyz;

    o.nrmWS = mul(float4(v.nrmL, 0.0f), gWorldInvTranspose).xyz;

    float4 posV = mul(posW, gView);
    o.posH = mul(posV, gProj);

    o.col = v.col;
    o.uv  = v.uv;
    return o;
}
