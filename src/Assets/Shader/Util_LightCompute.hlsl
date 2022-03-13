#ifndef __UTIL_LIGHTCOMPUTE_HLSL__
#define __UTIL_LIGHTCOMPUTE_HLSL__
#include "Util_Math.hlsl"
#include "Design_Res.hlsl"
/*
常用的光照符号
N:normalize(normal)

L：碰撞点->入射光源(Wi)
float3 L = normalize(Light.PosW - Point.PosW);

V：碰撞点->出射方向(Wo)
float3 V = normalize(Cam.PosW/Eye.PosW - Point.PosW)

H：半角向量
float3 H = normalize(L + V);

R：反射光方向
float3 R = normalize(2*N*(N * L) - L)
*/

//--------------Normal distribution function------------------
float LightCom_NormDisfunc_GGXTR(float3 N, float3 H, float3 a)//a衡量粗糙度
{
    float a2 = pow(a, 4);
    //float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    float denomToSquare = (NdotH2 * (a2 - 1.0f) + 1.0f);
    return a2 / (PI * denomToSquare * denomToSquare);
}

//--------------Fresnel equation------------------
float3 LightCom_Fresnel_GetF0(float3 albedo, float metalness)
{
    float F0_NON_METAL = 0.04;
    return lerp(F0_NON_METAL.rrr, albedo.rgb, metalness);
}

float3 LightCom_Fresnel_Schlick(float3 H, float3 V, float3 f0)
{
    float HdotV = max(dot(H, V), 0.0f);
    return f0 + (1.0f - f0) * pow(1.0f - HdotV, 5.0f);
}

float3 LightCom_Fresnel_Epic(float3 H, float3 V, float3 f0)
{
    float HdotV = max(dot(H, V), 0.0f);
    return f0 + (1.0f - f0) * exp2((-5.55473f * HdotV - 6.98316f) * HdotV);
}
float3 LightCom_Fresnel_SchlickRoughness(float3 N, float3 V, float3 f0, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float r1 = 1.0f - roughness;
    return f0 + (max(float3(r1, r1, r1), f0) - f0) * pow(1 - NdotV, 5.0f);
}   

//------------Geomertry function-------------------
float LightCom_Geofunc_SchlickGGX(float3 N, float3 V,float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float geo_Shadow = NdotV / (NdotV * (1 - k) + k); //视线导致的几何阴影（geometry shadowing）
    float geo_Objstruct = NdotL / (NdotL * (1 - k) + k); //光照导致的几何障碍（geometry obstruction）
    return geo_Shadow * geo_Objstruct;
}

float LightCom_Geofunc_SchlickGGX_Direct(float3 N, float3 V, float3 L, float a)//a衡量粗糙度
{
    float k = pow((a + 1), 2) / 8;
    return LightCom_Geofunc_SchlickGGX(N, V, L, k);
}

float LightCom_Geofunc_SchlickGGX_IBL(float3 N, float3 V, float3 L, float a)//a衡量粗糙度
{
    float k = pow(a, 2) / 2;
    return LightCom_Geofunc_SchlickGGX(N, V, L, k);
}

//----------------Gamma Correction------------
float3 LightCom_GammaCorrection(float3 color)
{
    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return color;
}

//------------BRDF-------------------
#define PREFILTER_MIP_LEVEL 5
float3 BRDF_CookTorrance_Specular(float3 N, float3 V, float3 L, float D,float3 F, float G)
{
    float NdotL = max(dot(N, L), 0.0f);
    float NdotV = max(dot(N, V), 0.0f);
    float3 specular = D * F * G / max(4.0f * NdotV * NdotL, 0.001f);
    return specular;
}

float3 BRDF_CookTorrance_Diffuse(float3 albedo)
{
    return albedo / PI;
}

float2 BRDF_SampleLUTMap(float3 N, float3 V, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float2 uv = float2(NdotV, roughness);
    uv = clamp(uv, float2(0.0f, 0.0f), float2(0.99f, 0.99f));
    return gbrdfLUTMap.Sample(gsamAnisotropicWrap, uv).rg;
}

float3 BRDF_SamplePrefilteredMap(float3 N, float3 V, float roughness)
{
    float roughnessLevel = roughness * PREFILTER_MIP_LEVEL;
    int fl = floor(roughnessLevel);
    int cl = ceil(roughnessLevel);
    float3 L = reflect(-V, N);
    float3 flSample = gPrefilteredMap.SampleLevel(gsamAnisotropicWrap, L, fl).rgb;
    float3 clSample = gPrefilteredMap.SampleLevel(gsamAnisotropicWrap, L, cl).rgb;
    float3 prefilterColor = lerp(flSample, clSample, (roughnessLevel - fl));
    return prefilterColor;
}
#endif

