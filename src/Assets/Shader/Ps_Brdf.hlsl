#ifndef __PS_BRDF_HLSL__
#define __PS_BRDF_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_LightCompute.hlsl"

float4 PSMain(VSOut_Default pin) : SV_TARGET
{
    float3 worldPos = pin.PosW.xyz;
    float3 albedo = pow(gAlbedoMapArray[gObjTexData[gObjTexCbIdx].idx_AlbedoMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb, 2.2); //配合伽马矫正
    float roughness = gRoughMapArray[gObjTexData[gObjTexCbIdx].idx_RoughMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    float metalness = gMetalMapArray[gObjTexData[gObjTexCbIdx].idx_MetalMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    float ao = gAOMapArray[gObjTexData[gObjTexCbIdx].idx_AOMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    
    float3 N;
    if (gObjTexData[gObjTexCbIdx].idx_NormalMap == 0)
    {
        N = pin.NormalW;
    }
    else
    {
        float3 normalMapSample = gNormalMapArray[gObjTexData[gObjTexCbIdx].idx_NormalMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb;
        N = SampleMap_NormalMap2WorldSpace(normalMapSample.rgb, normalize(pin.NormalW), pin.TangentW);
    }
    
    float3 V = normalize(gEyePosW - worldPos);
    float3 finalColor = 0.f;
    float shadowAmount = 1.f;
    float3 f0 = LightCom_Fresnel_GetF0(albedo, metalness);
    
    /*直接光 
    实时计算Brdf积分
    （离散相加模拟积分，但进入球面的光源只有区区可数的几个）
    */
    for (int i = 0; i < gDirLightCount; i++)
    {
        float3 L = normalize(-gDirLight[i].dir);
        float3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0);
        
        float D = LightCom_NormDisfunc_GGXTR(N,H,roughness);
        float3 F = LightCom_Fresnel_Epic(H,V,f0);
        float G = LightCom_Geofunc_SchlickGGX_Direct(N,V,L, roughness);
        float3 kS = F;
        float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);
        float3 diffuse = kD * BRDF_CookTorrance_Diffuse(albedo);
        float3 specular = kS * BRDF_CookTorrance_Specular(N, V, L, D, F, G);
        float3 brdfColor = (diffuse+specular) * NdotL * gDirLight[i].idensity * gDirLight[i].color.rgb * shadowAmount;
        
        finalColor = finalColor + brdfColor;
    }

    /*
    环境光 
    以CubeMap作为光源，需要真正的用很多点采样来模拟积分
    （但已经预渲染将球面采样计算部分储存到了纹理中，直接采样预渲染纹理即可）
    */
    float3 prefilteredColor = BRDF_SamplePrefilteredMap(N, V, roughness); //高光预渲染，储存的是高光公式Li光强部分
    float2 brdf = BRDF_SampleLUTMap(N, V, roughness); //高光预渲染，LUT纹理，x通道储存高光公式fr化简后和F0相乘的部分，y通道储存加法部分（详情见笔记-IBL-右边部分）
    float3 irradiance = gIrradianceMap.Sample(gsamAnisotropicWrap, N).rgb;//漫反射预渲染，储存的是光强

    float3 F = LightCom_Fresnel_SchlickRoughness(N, V, f0, roughness); //高光公式右侧化简后约去了DFG中的F，所以F要单独计算
    float3 kS = F;
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);

    /*                  
                           左侧                    右侧
                                          F   右侧第一 右侧第二
    */ 
    float3 diffuse = irradiance * albedo;
    float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    float3 ambient = (kD * diffuse + specular) * ao;
    
    finalColor += ambient;
    
    finalColor = LightCom_GammaCorrection(finalColor); //伽马矫正
    
    return float4(finalColor, 1.0f);
}


#endif