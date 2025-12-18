#ifndef COMMON_HLSLI
#define COMMON_HLSLI

static const float PI = 3.14159265f;

float3 SafeNormalize(float3 v)
{
    float len2 = dot(v, v);
    if (len2 < 1e-8f) return float3(0, 0, 1);
    return v * rsqrt(len2);
}

float Pow5(float x)
{
    float x2 = x * x;
    return x2 * x2 * x;
}

#endif // COMMON_HLSLI