#ifndef __PS_IBL_DIFFUSE_HLSL__
#define __PS_IBL_DIFFUSE_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_LightCompute.hlsl"

float4 PSMain(VSOut_PosL pin) : SV_TARGET
{
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 N = normalize(pin.PosL);
    
    //uint sampleNum = 4096u;
    //for (uint i = 0u; i < sampleNum; i++)
    //{
    //    float2 Xi = Random_Hammersley2d(i, sampleNum);
    //    float3 hemiSampleL_Tan = Sample_HemiSphere_Cos(Xi.x,Xi.y);
    //    float3 axi = cross(N, hemiSampleL_Tan);
    //    float NdotL = dot(N, hemiSampleL_Tan);
    //    float theta = acos(NdotL);
    //    float4 quat = Quat(axi,theta);
    //    float3 hemiSampleL_W = Quat_RotVec(quat, hemiSampleL_Tan);
    //    irradiance += gCubeMap.Sample(gsamAnisotropicWrap, hemiSampleL_W).xyz * cos(theta) * sin(theta);
    //}
    //irradiance *= PI * 1.0 / float(sampleNum);
    
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = cross(up, N);
    up = cross(N, right);

    float sampleDelta = 2 * PI / 50;//每圈采样50次
    float numSamples = 0.0f;
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 hemiSampleL_Tan = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = hemiSampleL_Tan.x * right + hemiSampleL_Tan.y * up + hemiSampleL_Tan.z * N;

            irradiance += gCubeMap.Sample(gsamAnisotropicWrap, sampleVec).rgb * cos(theta) * sin(theta);
            numSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0f / numSamples);
   
    
    return float4(irradiance, 1.0f);
}

#endif