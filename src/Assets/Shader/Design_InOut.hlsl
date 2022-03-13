#ifndef __DESIGN_INOUT_HLSL__
#define __DESIGN_INOUT_HLSL__

struct VSIn_Pos
{
    float3 PosL : POSITION;
};

struct VSOut_PosL
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

struct VSIn_Default
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentL : TANGENT;
#ifdef SKINNED
    float3 BoneWeights : WEIGHTS;
    uint4 BoneIndices : BONEINDICES;
#endif
};

struct VSOut_Default
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD; 
    float3 TangentW : TANGENT;
};


struct VSOut_Gbuffer
{
    float4 PosH : SV_POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentW : TANGENT;
};

struct VSOut_ScreenQuat
{
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION;
    float2 TexC : TEXCOORD;
};

#endif