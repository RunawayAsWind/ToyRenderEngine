#ifndef __UTIL_SAMPLE_HLSL__
#define __UTIL_SAMPLE_HLSL__
#include "Util_Math.hlsl"

/*
半球面采样函数
输入u,v是[0,1]的均匀随机数 return：采样方向的单位向量
theta（pitch） = arccos(1-2u);
phi（yaw） = 2*pi*v;

或
theta（pitch）= 2arccos(sqrt(1-u))
phi（yaw= 2*pi*v;
*/
float3 Sample_Sphere_Unifom(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = 1 - 2 * u; 
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
    
float3 Sample_Sphere_Cos(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = 2 * sqrt(1 - u); 
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float3 Sample_HemiSphere_Unifom(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = 1 - u; //原式0<arccos(1-2u)<pi 因为是半球，当2u<1的时候，theta在0~90  
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
    
float3 Sample_HemiSphere_Cos(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = sqrt(1 - u); //原式0<2arccos(sqrt(1-u))<pi 因为是半球，不等式两边除2，0<arccos(sqrt(1-u))<pi/2,满足theta在0~90  
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

//粗糙度越小采样结果越靠近半球的n
float3 Sample_HemiSphere_Cos_GGX(float u, float v, float3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * u;
    float cosTheta = sqrt((1.0 - v) / (1.0 + (a * a - 1.0) * v));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);

    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}


float3 SampleMap_NormalMap2WorldSpace(float3 normalMapSampleRes, float3 unitNormalW, float3 tangentW)
{ // Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSampleRes - 1.0f;
    // Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);//保证切线法线相互垂直
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    // Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);
    return normalize(bumpedNormalW);
}

#endif