#ifndef __PS_BRDF_HLSL__
#define __PS_BRDF_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_LightCompute.hlsl"

float4 PSMain(VSOut_Default pin) : SV_TARGET
{
    float3 worldPos = pin.PosW.xyz;
    float3 albedo = pow(gAlbedoMapArray[gObjTexData[gObjTexCbIdx].idx_AlbedoMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb, 2.2); //���٤�����
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
    
    /*ֱ�ӹ� 
    ʵʱ����Brdf����
    ����ɢ���ģ����֣�����������Ĺ�Դֻ�����������ļ�����
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
    ������ 
    ��CubeMap��Ϊ��Դ����Ҫ�������úܶ�������ģ�����
    �����Ѿ�Ԥ��Ⱦ������������㲿�ִ��浽�������У�ֱ�Ӳ���Ԥ��Ⱦ�����ɣ�
    */
    float3 prefilteredColor = BRDF_SamplePrefilteredMap(N, V, roughness); //�߹�Ԥ��Ⱦ��������Ǹ߹⹫ʽLi��ǿ����
    float2 brdf = BRDF_SampleLUTMap(N, V, roughness); //�߹�Ԥ��Ⱦ��LUT����xͨ������߹⹫ʽfr������F0��˵Ĳ��֣�yͨ������ӷ����֣�������ʼ�-IBL-�ұ߲��֣�
    float3 irradiance = gIrradianceMap.Sample(gsamAnisotropicWrap, N).rgb;//������Ԥ��Ⱦ��������ǹ�ǿ

    float3 F = LightCom_Fresnel_SchlickRoughness(N, V, f0, roughness); //�߹⹫ʽ�Ҳ໯���Լȥ��DFG�е�F������FҪ��������
    float3 kS = F;
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);

    /*                  
                           ���                    �Ҳ�
                                          F   �Ҳ��һ �Ҳ�ڶ�
    */ 
    float3 diffuse = irradiance * albedo;
    float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    float3 ambient = (kD * diffuse + specular) * ao;
    
    finalColor += ambient;
    
    finalColor = LightCom_GammaCorrection(finalColor); //٤�����
    
    return float4(finalColor, 1.0f);
}


#endif