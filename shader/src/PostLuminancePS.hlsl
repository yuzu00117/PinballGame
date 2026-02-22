// PostLuminancePS.hlsl
// レンダーターゲットから高輝度なピクセルだけを抽出する

Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

struct PSIn
{
    float4 posH : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

// しきい値（これより明るい部分を抽出）
static const float LUM_THRESHOLD = 1.0f; // 閾値を超えた分だけを抽出するため1.0に戻す

float4 main(PSIn i) : SV_TARGET
{
    float4 col = gTex.Sample(gSamp, i.uv);
    
    // RGBの最大値
    float maxCol = max(col.r, max(col.g, col.b));
    
    if (maxCol > LUM_THRESHOLD)
    {
        // 閾値を超えた「溢れた光成分」だけを抽出する
        // 例: 輝度が1.5なら、1.0を超えた0.5の部分だけがぼかしの元になる（自然なブルーム）
        float3 bloomPart = col.rgb - LUM_THRESHOLD;
        return float4(bloomPart, 1.0f);
    }
    
    return float4(0, 0, 0, 1);
}
