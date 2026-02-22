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
static const float LUM_THRESHOLD = 1.2f;

float4 main(PSIn i) : SV_TARGET
{
    float4 col = gTex.Sample(gSamp, i.uv);
    
    // RGBの最大値
    float maxCol = max(col.r, max(col.g, col.b));
    
    if (maxCol > LUM_THRESHOLD)
    {
        // 閾値を超えた色だけを取り出す（発光部分を自然にぼかすため）
        return float4(col.rgb, 1.0f);
    }
    
    return float4(0, 0, 0, 1);
}
