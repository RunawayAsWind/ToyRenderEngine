#ifndef __VS_SCREENQUAT_HLSL__
#define __VS_SCREENQUAT_HLSL__
#include "Design_Res.hlsl"

static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};

VSOut_ScreenQuat VSMain(uint vid : SV_VertexID)
{
    VSOut_ScreenQuat vout;
    vout.TexC = gTexCoords[vid];

    // Quad covering screen in NDC space.
    vout.PosH = float4(2.0f * (vout.TexC.x - 0.5f), 2.0f * (0.5f - vout.TexC.y), 1.0f, 1.0f);
    
    // Transform quad corners to view space far plane.
    float4 ph = mul(vout.PosH, gInvProj);
    vout.PosV = ph.xyz / ph.w;
    
    return vout;
}

#endif