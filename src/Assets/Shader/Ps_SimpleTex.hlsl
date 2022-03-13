#ifndef __PS_SIMPLETEX_HLSL__
#define __PS_SIMPLETEX_HLSL__
#include "Design_Res.hlsl"

float4 PSMain(VSOut_Default pin) : SV_TARGET
{
    float3 albedo = gAlbedoMapArray[gObjTexData[gObjTexCbIdx].idx_AlbedoMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb;
    
    float3 N = normalize(pin.NormalW);
    
    float3 finalColor = 0.f;
    for (int i = 0; i < gDirLightCount; i++)
    {
        float3 L = normalize(-gDirLight[i].dir);
        float NdotL = max(dot(N, L), 0.0);
        float3 diffuse = NdotL * gDirLight[i].color.rgb;
        finalColor = finalColor + diffuse * albedo;
    }
    
    return float4(albedo, 1.0f);
}

#endif