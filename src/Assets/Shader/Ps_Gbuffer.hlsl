#ifndef __PS_GBUFFER_HLSL__
#define __PS_GBUFFER_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_NormalCompress.hlsl"

struct PSOut
{   
    //float2 normal : SV_TARGET0;
    float4 normal : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 metalRoughAo : SV_TARGET2;
};

PSOut PSMain(VSOut_Gbuffer pin) : SV_TARGET
{
    PSOut pout;

    if(gObjTexData[gObjTexCbIdx].idx_NormalMap==0)
    {
        /*
        float3 encodeN = NormalCompress_UnitVectorToSpTrans(normalize(pin.NormalW));
        pout.normal = encodeN;
        */
        pout.normal.xyz = normalize(pin.NormalW);
    }
    else
    {
        float3 normalMapSample = gNormalMapArray[gObjTexData[gObjTexCbIdx].idx_NormalMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb;
        float3 sampleN = SampleMap_NormalMap2WorldSpace(normalMapSample.rgb, normalize(pin.NormalW), pin.TangentW);
        
        /*
        float2 encodeN = NormalCompress_UnitVectorToSpTrans(sampleN);
        pout.normal = encodeN;
        */
        pout.normal.xyz = sampleN;
        
    }

    pout.albedo = float4(gAlbedoMapArray[gObjTexData[gObjTexCbIdx].idx_AlbedoMap].Sample(gsamAnisotropicWrap, pin.TexC).rgb, 1);
    pout.metalRoughAo.x = gMetalMapArray[gObjTexData[gObjTexCbIdx].idx_MetalMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    pout.metalRoughAo.y = gRoughMapArray[gObjTexData[gObjTexCbIdx].idx_RoughMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    pout.metalRoughAo.z = gAOMapArray[gObjTexData[gObjTexCbIdx].idx_AOMap].Sample(gsamAnisotropicWrap, pin.TexC).r;
    pout.metalRoughAo.w = gRenderIdx / 255.0f;

    return pout;
}

#endif