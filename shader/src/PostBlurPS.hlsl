// PostBlurPS.hlsl
// ガウシアンぼかし用シェーダー

Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

cbuffer BlurCB : register(b0)
{
    float2 TexelSize; // テクスチャの1ピクセルあたりのサイズ (1/W, 1/H)
    float2 Dir;       // ぼかす方向 (1,0) もしくは (0,1)
};

struct PSIn
{
    float4 posH : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

// 7タップのガウシアンフィルタの重み
static const float WEIGHTS[7] = {
    0.00598, 0.060626, 0.241843, 0.383103, 0.241843, 0.060626, 0.00598
};

float4 main(PSIn i) : SV_TARGET
{
    float4 color = 0;
    
    // 現在のピクセルを中心に、-3 ～ +3 ピクセルの範囲でサンプリング
    [unroll]
    for (int j = -3; j <= 3; ++j)
    {
        float2 offset = Dir * TexelSize * (float)j;
        color += gTex.Sample(gSamp, i.uv + offset) * WEIGHTS[j + 3];
    }
    
    return float4(color.rgb, 1.0f);
}
