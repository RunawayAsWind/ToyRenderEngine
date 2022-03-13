#ifndef __PS_CUBEMAP_HLSL__
#define __PS_CUBEMAP_HLSL__
#include "Design_Res.hlsl"

float4 PSMain(VSOut_PosL pin) : SV_TARGET
{
    return gCubeMap.Sample(gsamAnisotropicWrap, pin.PosL);
}

#endif