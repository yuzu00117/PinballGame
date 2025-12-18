// BaseLitPS.hlsl
// 目的：Directional/Point/Spot を合成して BaseColor でライティング（NormalMap/Rimなし）

// ---- Common ----
float3 SafeNormalize(float3 v)
{
    float len2 = dot(v, v);
    if (len2 < 1e-8f) return float3(0, 0, 1);
    return v * rsqrt(len2);
}

// ---- Lights ----
struct DirectionalLight
{
    float3 directionWS; // 光が進む方向（例：下向きなら 0,-1,0）
    float  intensity;
    float3 color;
    float  pad0;
};

struct PointLight
{
    float3 positionWS;
    float  range;
    float3 color;
    float  intensity;
};

struct SpotLight
{
    float3 positionWS;
    float  range;
    float3 directionWS; // 光が進む方向
    float  innerCos;    // cos(innerAngle)
    float  outerCos;    // cos(outerAngle)
    float3 color;
    float  intensity;
};

float AttenuationDistance(float dist, float range)
{
    float x = saturate(1.0f - dist / max(range, 1e-4f));
    return x * x;
}

float AttenuationSpot(float3 lightToPointDir, float3 spotDirWS, float innerCos, float outerCos)
{
    float cd = dot(SafeNormalize(lightToPointDir), SafeNormalize(spotDirWS));
    return smoothstep(outerCos, innerCos, cd);
}

struct LightSample
{
    float3 L;        // 点→ライト方向（正規化）
    float3 radiance; // 色*強度*減衰
};

LightSample SampleDirectional(DirectionalLight lt)
{
    LightSample s;
    s.L = SafeNormalize(-lt.directionWS);
    s.radiance = lt.color * lt.intensity;
    return s;
}

LightSample SamplePoint(PointLight lt, float3 Pws)
{
    LightSample s;
    float3 toL = lt.positionWS - Pws;
    float dist = length(toL);
    s.L = (dist > 1e-4f) ? (toL / dist) : float3(0,1,0);
    float att = AttenuationDistance(dist, lt.range);
    s.radiance = lt.color * lt.intensity * att;
    return s;
}

LightSample SampleSpot(SpotLight lt, float3 Pws)
{
    LightSample s;
    float3 toL = lt.positionWS - Pws;
    float dist = length(toL);
    s.L = (dist > 1e-4f) ? (toL / dist) : float3(0,1,0);

    float attD = AttenuationDistance(dist, lt.range);

    // ライト→点 方向でスポット角評価
    float3 lightToPointDir = SafeNormalize(Pws - lt.positionWS);
    float attS = AttenuationSpot(lightToPointDir, lt.directionWS, lt.innerCos, lt.outerCos);

    s.radiance = lt.color * lt.intensity * attD * attS;
    return s;
}

// ---- Material ----
cbuffer CBMaterial : register(b2)
{
    float3 gBaseColor;
    float  gRoughness; // 今は未使用（将来用）
    float  gMetallic;  // 今は未使用（将来用）
    float3 gEmissive;  // 今は未使用（将来用）
};

// ---- Scene / Lights ----
cbuffer CBScene : register(b1)
{
    float3 gCameraPosWS;
    int    gNumPoint;
    int    gNumSpot;
    float2 padScene;
};

cbuffer CBLights : register(b3)
{
    DirectionalLight gDir;
    PointLight gPoint[16];
    SpotLight  gSpot[16];
};

// ---- IO ----
struct PSIn
{
    float4 posH   : SV_POSITION;
    float3 posWS  : TEXCOORD0;
    float3 nrmWS  : TEXCOORD1;
    float2 uv     : TEXCOORD2; // 今は未使用（将来テクスチャ用）
};

// ---- BRDF（最小：Lambert）----
float3 ShadeLambert(float3 N, float3 L, float3 radiance, float3 baseColor)
{
    float ndl = saturate(dot(SafeNormalize(N), SafeNormalize(L)));
    return baseColor * ndl * radiance;
}

float4 main(PSIn i) : SV_TARGET
{
    float3 P = i.posWS;
    float3 N = SafeNormalize(i.nrmWS);
    float3 V = SafeNormalize(gCameraPosWS - P); // 今は未使用（将来スペキュラ用）

    float3 col = 0;

    // Directional
    {
        LightSample s = SampleDirectional(gDir);
        col += ShadeLambert(N, s.L, s.radiance, gBaseColor);
    }

    // Point
    [loop]
    for (int p = 0; p < gNumPoint; ++p)
    {
        LightSample s = SamplePoint(gPoint[p], P);
        col += ShadeLambert(N, s.L, s.radiance, gBaseColor);
    }

    // Spot
    [loop]
    for (int sidx = 0; sidx < gNumSpot; ++sidx)
    {
        LightSample s = SampleSpot(gSpot[sidx], P);
        col += ShadeLambert(N, s.L, s.radiance, gBaseColor);
    }

    return float4(col, 1.0f);
}
