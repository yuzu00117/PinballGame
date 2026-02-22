// PostCompositePS.hlsl
// メイン画像とブルーム画像を合成してLDR化する

Texture2D gMainTex  : register(t0); // メインカラー (HDR)
Texture2D gBloomTex : register(t1); // ぼかした光 (HDR)
SamplerState gSamp  : register(s0);

struct PSIn
{
    float4 posH : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

// ACES ToneMapping (関数)
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float4 main(PSIn i) : SV_TARGET
{
    // メインカラー取得
    float4 mainCol  = gMainTex.Sample(gSamp, i.uv);
    // ブルームカラー取得（加算合成向け）
    float4 bloomCol = gBloomTex.Sample(gSamp, i.uv);
    
    // ブルーム強度（必要に応じて調整）
    float bloomIntensity = 0.5f;
    // HDR合成
    float3 hdrCol = mainCol.rgb + (bloomCol.rgb * bloomIntensity);
    
    // TONE MAPPING (HDR -> LDR変換)
    // float3 ldrCol = ACESFilm(hdrCol);
    float3 ldrCol = saturate(hdrCol);
    
    return float4(ldrCol, 1.0f);
}
