#pragma once
#include "RFbxNodeDataGet.h"
#include "RFbxAnimDataGet.h"
#include "FbxHelper/Common.h"
#include <queue>

struct RFbxMidData
{
    std::vector<uint64_t> mPureMeshId;
    std::vector<RDx12::PureMesh<RDx12::VertexData>> mPureMeshVec;

	std::vector<uint64_t> mSkinMeshId;
    std::vector<RDx12::PureMesh<RDx12::SkinVertexData>> mSkinMeshVec;
    std::vector<DirectX::XMFLOAT4X4> mSkinMeshBindWorldMat;

    std::vector<uint64_t> mLinkNodeId;
    RDx12::Skeleton skeleton;

    RFbxNode rootNode;

	void GetSkeleton(FbxScene* lScene);
	void GetMesh(FbxScene* lScene);
};

class RFbxMiddle
{
public:
	static RFbxMidData GetMidData(std::string path);
private:
	static RFbxNode GetMidNodeRcs(FbxNode* fbxNode, RFbxMidData* fbxMidData, FbxScene* lScene, int depth);
};