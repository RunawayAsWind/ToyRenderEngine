#ifndef __PS_IBL_PREFILTERED_HLSL__
#define __PS_IBL_PREFILTERED_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_LightCompute.hlsl"

float4 PSMain(VSOut_PosL pin) : SV_TARGET
{
    float roughness = cbPerObjectPad1;
    const uint NumSamples = 1024u;
    
    float3 N = normalize(pin.PosL);
    float3 R = N;
    float3 V = R;

    float3 prefilteredColor = float3(0.f, 0.f, 0.f);
    float totalWeight = 0.f;
    for (uint i = 0u; i < NumSamples; ++i)
    {
        float2 Xi = Random_Hammersley2d(i, NumSamples);
        float3 H = Sample_HemiSphere_Cos_GGX(Xi.x,Xi.y, N, roughness);
        float3 L = normalize(reflect(-V, H));
        float NdotL = max(dot(N, L), 0.0f);
        if (NdotL > 0.f)
        {
            prefilteredColor += gCubeMap.Sample(gsamAnisotropicWrap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    return float4(prefilteredColor, 1.0f);
}

#endif