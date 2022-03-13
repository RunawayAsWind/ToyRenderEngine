#ifndef __DESIGN_BASE_HLSL__
#define __DESIGN_BASE_HLSL__
#include "Design_InOut.hlsl"

struct Light
{
    float3 pos;
    float idensity;
    float4 color;
    float3 dir;
    float lValue;
    float qValue;
    float fallStart;
    float fallEnd;
    float spotThetaRad;
};

struct ObjTexCb
{
    uint idx_AlbedoMap;
    uint idx_NormalMap;
    uint idx_DisplacementMap;
    uint idx_SpecularMap;
    uint idx_RoughMap;
    uint idx_MetalMap;
    uint idx_HeightMap;
    uint idx_AOMap;
};

cbuffer PassCb : register(b0)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gProjTex;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float2 gNearSize;
    float2 gFarSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
};

cbuffer ObjCb : register(b1)
{
    float4x4 gWorld;
    float4x4 gNormalWorld;
    float4x4 gTexTransform;
    int gRenderIdx;
    int gObjTexCbIdx;
    int gIsSelected;
    int gPadding2;
};

cbuffer LightCb : register(b2)
{
    Light gDirLight[4];
    Light gPintLight[16];
    Light gSpotLight[16];
    uint gDirLightCount;
    uint gPointLightCount;
    uint gSpotLightCount;
    uint gPadding;
}

//����ʹ������float gBlurWeight[16]�����飬����HLSL������������е�ÿ���� float����������һ��16�ֽڵĲ�ۣ��൱��float4�Ŀռ䣩��16*float4�Ŀռ䣬ͨ���±�ȡֵ��ȡ��û����ֵ������
//https://stackoverflow.com/questions/14711763/hlsl-float-array-packing-in-constant-buffer
//https://geidav.wordpress.com/2013/03/05/hidden-hlsl-performance-hit-accessing-unpadded-arrays-in-constant-buffers/
//https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
//ʹ��float4��float3��float2���򵥸���float������float��������
cbuffer BlurCoreCb : register(b3)
{
    //uint gBlurRadius;
    //float gBlurWeight[16]
    
    // We cannot have an array entry in a constant buffer that gets mapped onto
	// root constants, so list each element.  
    int gBlurRadius;
	// Support up to 11 blur weights.
    float gBlurWeight0;
    float gBlurWeight1;
    float gBlurWeight2;
    float gBlurWeight3;
    float gBlurWeight4;
    float gBlurWeight5;
    float gBlurWeight6;
    float gBlurWeight7;
    float gBlurWeight8;
    float gBlurWeight9;
    float gBlurWeight10;
};

cbuffer SkinnedCb : register(b4)
{
    float4x4 gBoneTransforms[96];
}
#endif