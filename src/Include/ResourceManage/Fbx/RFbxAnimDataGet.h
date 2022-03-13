#pragma once
#include <fbxsdk.h>
#include <RDx12/Geomertry/PureMesh.h>
#include <RDx12/Geomertry/Skeleton.h>

//Use For Fbx
struct RFbxIdxAndWeight
{
	RDx12::IndexType mIdx;
	float mWeight;
};

struct RFbxLink
{
	uint64_t mLinkNodeUniqueId;
	std::vector<RFbxIdxAndWeight> mIdxAndWeight;
	DirectX::XMFLOAT4X4 mMeshBindWorldMat = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mLinkNodeBindWorldMat = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4 mLinkNodeBindWorldQuat{0,0,0,1};
};

struct RFbxKv
{
	int mTime;
	float mVal;
};

struct RFbxCurve
{
	std::vector<RFbxKv> mKvVec;
	float Interpolate(float t, float defaultRes);
};

class RFbxAnimDataGet
{
public:
	static RDx12::AnimCurve GetAnimCurve(FbxNode* pNode, FbxAnimLayer* pAnimLayer);
	static RFbxLink GetSkinLink(FbxCluster* lCluster);
private:
	static RFbxCurve GetCurve(FbxAnimCurve* lAnimCurve);
	static void GetDurationAndInterval(FbxAnimCurve* lAnimCurve, int& maxFrameCount, FbxTime::EMode& eMode);
	static RDx12::AnimCurve GetAnimCurve(FbxNode* pNode, int maxFrameCount, FbxTime::EMode eMode, bool yUporzUp);
};

