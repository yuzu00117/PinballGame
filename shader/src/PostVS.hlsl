// PostVS.hlsl
// フルスクリーン描画用頂点シェーダー（頂点バッファなし）

struct PSIn
{
    float4 posH : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

PSIn main(uint vI : SV_VertexID)
{
    PSIn o;
    // SV_VertexID (0,1,2) から フルスクリーンになる巨大な三角形を生成
    o.uv = float2((vI << 1) & 2, vI & 2);
    o.posH = float4(o.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return o;
}
