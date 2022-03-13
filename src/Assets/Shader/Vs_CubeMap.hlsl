#ifndef __VS_CUBEMAP_HLSL__
#define __VS_CUBEMAP_HLSL__
#include "Design_Res.hlsl"

VSOut_PosL VSMain(VSIn_Pos vin)
{
    VSOut_PosL vout;

	// Use local vertex position as cubemap lookup vector.
    vout.PosL = vin.PosL;
	
	// Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	// Always center sky about camera.
    posW.xyz += gEyePosW;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    vout.PosH = mul(posW, gViewProj).xyww;
	
    return vout;
}

#endif