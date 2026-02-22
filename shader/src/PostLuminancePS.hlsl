// PostLuminancePS.hlsl
// レンダーターゲットから高輝度なピクセルだけを抽出する

Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

struct PSIn
{
    float4 posH : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

// 抽出する輝度の閾値（要調整）
static const float LUM_THRESHOLD = 1.0f;

float4 main(PSIn i) : SV_TARGET
{
    float4 col = gTex.Sample(gSamp, i.uv);
    
    // RGBの最大成分が閾値を超えているか
    float maxCol = max(col.r, max(col.g, col.b));
    
    if (maxCol > LUM_THRESHOLD)
    {
        // 閾値を超えた分を取り出すか、そのまま全色出すか
        // 単純化のためそのまま出力する
        return float4(col.rgb, 1.0f);
    }
    
    return float4(0, 0, 0, 1);
}
