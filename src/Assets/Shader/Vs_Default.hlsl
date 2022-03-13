#ifndef __VS_DEFAULT_HLSL__
#define __VS_DEFAULT_HLSL__
#include "Design_Res.hlsl"

VSOut_Default VSMain(VSIn_Default vin)
{
    VSOut_Default vout;
    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(mul(vout.PosW, gView), gProj);
    vout.NormalW = mul(vin.NormalL, (float3x3) gNormalWorld);
    vout.TexC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform).xy;
    vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
    return vout;
}

#endif