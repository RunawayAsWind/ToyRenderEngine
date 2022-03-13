#ifndef __DESIGN_RES_HLSL__
#define __DESIGN_RES_HLSL__
#include "Design_Base.hlsl"

/*Brdf*/
TextureCube gCubeMap : register(t0, space0);
TextureCube gIrradianceMap : register(t1, space0);
TextureCube gPrefilteredMap : register(t2, space0);
Texture2D gbrdfLUTMap : register(t3, space0);

Texture2D gDepthMap : register(t0, space1);
Texture2D gSsaoBuffer : register(t1, space1);
/*GBuffer*/
Texture2D gNormalBuffer : register(t2, space1);//normalW.xy 1 1
Texture2D gAlbedoBuffer : register(t3, space1);//albedo.xyz 1
Texture2D gMetalRoughAoBuffer : register(t4, space1); //metal rough ao renderIdx

Texture2D gBlurInput : register(t0, space2);RWTexture2D<float4> gBlurOutput : register(u0,space0);
Texture2D gSobelInput : register(t1, space2);RWTexture2D<float4> gSobelOutput : register(u1, space0);

/*场景所需的所有材质*/
StructuredBuffer<ObjTexCb> gObjTexData : register(t0, space15);

/*ObjTexArray 场景所需的所有物体纹理*/
Texture2D gAlbedoMapArray[20] : register(t0, space16);
Texture2D gNormalMapArray[20] : register(t0, space17);
texture2D gDisplacementMapArray[20] : register(t0, space18);
Texture2D gSpecularMapArray[20] : register(t0, space19);
Texture2D gRoughMapArray[20] : register(t0, space20);
Texture2D gMetalMapArray[20] : register(t0, space21);
Texture2D gHeightMapArray[20] : register(t0, space22);
Texture2D gAOMapArray[20] : register(t0, space23);

/*
Sampler
*/
SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerState gsamDepthMapSam : register(s6);
#endif