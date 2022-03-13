#ifndef __PS_SCREENQUAT_HLSL__
#define __PS_SCREENQUAT_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_LightCompute.hlsl"
#include "Util_NormalCompress.hlsl"

float4 PSMain(VSOut_ScreenQuat pin) : SV_TARGET
{
    float sobel = gSobelInput.Sample(gsamAnisotropicWrap, pin.TexC).x;
    if (sobel < 0.01f)
    {
        return float4(0.3, 0.9, 1, 1);
    }
    
    float depthPercent = NdcDepthToViewDepth(gDepthMap.Sample(gsamDepthMapSam, pin.TexC).r,gProj) / gFarZ;
    float3 worldPos = mul(float4(pin.PosV * depthPercent, 1), gInvView).xyz;
     
    /*
    float2 encodeN = gNormalBuffer.Sample(gsamAnisotropicWrap, pin.TexC).xy;
    float3 N = NormalCompress_SpTransToUnitVector(encodeN);
    */
    float3 N = gNormalBuffer.Sample(gsamAnisotropicWrap, pin.TexC).xyz;

    float3 albedo = gAlbedoBuffer.Sample(gsamAnisotropicWrap, pin.TexC).xyz;
    
    float4 metalRoughAo_Res = gMetalRoughAoBuffer.Sample(gsamAnisotropicWrap, pin.TexC);
    float metalness = metalRoughAo_Res.x;
    float roughness = metalRoughAo_Res.y;
    float ao = metalRoughAo_Res.z;
    uint renderIdx = metalRoughAo_Res.w * 255;
    
    float ssAo = gSsaoBuffer.Sample(gsamAnisotropicWrap, pin.TexC).x;
    float blurssAO = gBlurInput.Sample(gsamAnisotropicWrap, pin.TexC).x;
    
    float3 finalColor = albedo; //Ϊ�˱�֤Ps_CubeMap��ͬʱ�����ӳ���Ⱦ��ǰ����Ⱦ�����ֱ�Ӱ�Skybox��Ⱦ�����ӳ���Ⱦ��Albedo�ϣ���û����Shader�и���renderIdx���޷����к����ķ�֧�������������Ĭ�ϸ�ֵalbedo
    if (renderIdx == 1)
    {
        float3 PbrColor = 0;
        
        albedo = pow(albedo, 2.2);
        float3 V = normalize(gEyePosW - worldPos);
   
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
      
            float D = LightCom_NormDisfunc_GGXTR(N, H, roughness);
            float3 F = LightCom_Fresnel_Epic(H, V, f0);
            float G = LightCom_Geofunc_SchlickGGX_Direct(N, V, L, roughness);
            float3 kS = F;
            float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);
            float3 diffuse = kD * BRDF_CookTorrance_Diffuse(albedo);
            float3 specular = kS * BRDF_CookTorrance_Specular(N, V, L, D, F, G);
            float3 brdfColor = (diffuse + specular) * NdotL * gDirLight[i].idensity * gDirLight[i].color.rgb * shadowAmount;
      
            PbrColor = PbrColor + brdfColor;
        }

        /*
        ������ 
        ��CubeMap��Ϊ��Դ����Ҫ�������úܶ�������ģ�����
        �����Ѿ�Ԥ��Ⱦ������������㲿�ִ��浽�������У�ֱ�Ӳ���Ԥ��Ⱦ�����ɣ�
        */
        float3 prefilteredColor = BRDF_SamplePrefilteredMap(N, V, roughness); //�߹�Ԥ��Ⱦ��������Ǹ߹⹫ʽLi��ǿ����
        float2 brdf = BRDF_SampleLUTMap(N, V, roughness); //�߹�Ԥ��Ⱦ��LUT����xͨ������߹⹫ʽfr������F0��˵Ĳ��֣�yͨ������ӷ����֣�������ʼ�-IBL-�ұ߲��֣�
        float3 irradiance = gIrradianceMap.Sample(gsamAnisotropicWrap, N).rgb; //������Ԥ��Ⱦ��������ǹ�ǿ

        float3 F = LightCom_Fresnel_SchlickRoughness(N, V, f0, roughness); //�߹⹫ʽ�Ҳ໯���Լȥ��DFG�е�F������FҪ��������
        float3 kS = F;
        float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);

        /*                  
                               ���                    �Ҳ�
                                              F   �Ҳ��һ �Ҳ�ڶ�
        */ 
        float3 diffuse = irradiance * albedo;
        float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

        float3 ambient = (kD * diffuse + specular) * ao * blurssAO;
    
        PbrColor += ambient;
    
        finalColor = LightCom_GammaCorrection(PbrColor); //٤�����
    }
    if (renderIdx == 0)
    {
        float3 SimpleTexColor = 0;
        for (int i = 0; i < gDirLightCount; i++)
        {
            float3 L = normalize(-gDirLight[i].dir);
            float NdotL = max(dot(N, L), 0.0);
            float3 diffuse = NdotL * gDirLight[i].color.rgb;
            SimpleTexColor = SimpleTexColor + diffuse * albedo;
        }
    }
    
    return float4(finalColor, 1);
    //return float4(ssAo.x,ssAo.x,ssAo.x, 1);
}

#endif