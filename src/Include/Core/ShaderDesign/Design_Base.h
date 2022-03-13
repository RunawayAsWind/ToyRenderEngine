#pragma once
#include<RDx12/Util/D12MathUtil.h>

struct Light
{
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	float idensity = 1;
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	DirectX::XMFLOAT3 dir = { 1,1,1 };
	float lValue = 0.09;
	float qValue = 0.032;
	float fallStart = 0;
	float fallEnd = 50;
	float spotThetaRad = RDx12::MathHelper::Deg2Rad(30);
};

static const std::vector<std::string> Navigation_ObjTexCb = { "AlbedoMap","NormalMap","DisplacementMap","SpecularMap","RoughMap","MetalMap","HeightMap","AOMap" };
struct ObjTexCb
{
	UINT hCpuIdx[8] = {0};
};

//每帧所有物体共用的常量缓冲，包括摄像机产生的VP矩阵，研究的位置
struct PassCb
{
	DirectX::XMFLOAT4X4 gView = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gInvView = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gProj = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gInvProj = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gViewProj = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gInvViewProj = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gProjTex = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT3 gEyePosW = { 0, 0, 0 };
	float cbPerObjectPad1 = 0.0f;                        //这个变量仅仅是为了和上面的FLOAT3构成一个FLOAT4的大小，因为Constansbuff有特殊的硬件要求，大小必须是256的整数倍，参见龙书pdf 6.6章节
	DirectX::XMFLOAT2 gRenderTargetSize = { 0, 0 };     //渲染的目标纹理的尺寸
	DirectX::XMFLOAT2 gInvRenderTargetSize = { 0, 0 };  //渲染的目标纹理的尺寸的倒数
	DirectX::XMFLOAT2 gNearSize = { 0, 0 };             //视锥体近平面尺寸
	DirectX::XMFLOAT2 gFarSize = { 0, 0 };              //远平面尺寸
	float gNearZ = 0;
	float gFarZ = 0;
	float gTotalTime = 0;
	float gDeltaTime = 0;
};

//每帧每个物体间不同的常量缓冲
struct ObjCb
{
	DirectX::XMFLOAT4X4 gWorld = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gNormalWorld = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 gTexTransform = RDx12::MathHelper::Identity4x4();
	int gRenderIdx = 0;
	int gObjTexCbIdx = 0;
	int gIsSelected = 0;
	int gPadding2 = 0;
};

struct LightCb
{
	Light gDirLight[4];
	Light gPointLight[16];
	Light gSpotLight[16];
	UINT gDirLightCount = 0;
	UINT gPointLightCount = 0;
	UINT gSpotLightCount = 0;
	UINT gPadding = 0;
};

struct BlurCoreCb
{
	int gBlurRadius = 0;
	float gBlurWeight[11] = { 0 };
};

#define BoneMaxNum 96
struct SkinnedCb
{
	DirectX::XMFLOAT4X4 gBoneTransforms[BoneMaxNum];
};

PassCb GetPassCb(
    DirectX::XMFLOAT3 gEyePosW,
    DirectX::XMMATRIX gView,
    DirectX::XMMATRIX gProj,
    DirectX::XMFLOAT2 gRenderTargetSize,
    DirectX::XMFLOAT2 gNearSize,
    float gNearZ,
    float gFarZ,
    float gTotalTime,
    float gDeltaTime
);

ObjCb GetObjCb(
    DirectX::XMMATRIX gWorld,
    DirectX::XMMATRIX gTexTransform,
	int gRenderIdx,
	int gObjTexCbIdx,
	int gIsSelected,
	int gPadding2
);

LightCb GetLightCb(
    std::vector<Light>& directLightVec,
    std::vector<Light>& pointLightVec,
    std::vector<Light>& spotLightVec);

static const std::vector<std::string> Navigation_RenderLayer = { "NonePbr","Pbr","Sky","Select","NoSelect"};
enum class RenderLayer : int
{
	NonePbr = 0,
	Pbr,
	Sky,
	Select,
	NotSelect,
	Count
};
enum class CameraLayer : int
{
	Main = 0,
	Count
};