#pragma once
#include "RFbxAnimDataGet.h"

struct RFbxNode
{
    std::string mName;
    uint64_t mUniqueId;
    FbxNodeAttribute::EType mNodeType;
    float mOffsetPosX, mOffsetPosY, mOffsetPosZ;
    float mOffsetEulerX, mOffsetEulerY, mOffsetEulerZ;
    float mOffsetScaleX, mOffsetScaleY, mOffsetScaleZ;
    std::vector<RFbxNode> mChild;

    /*Animation ��һά��AnimStack��� ���ڶ�ά��AnimLayer���  val��trans����*/
    std::vector<std::vector<RDx12::AnimCurve>> mAnim;

    /*If Type = eMesh*/
    int mMeshIdx=-1;
    int mSkinMeshIdx=-1;
    /*��һά��skin��ţ��ڶ�ά��cluster��ţ�val��Link����*/
    std::vector<std::vector<RFbxLink>> mSkin;
};

class RFbxNodeDataGet
{
public:
	static RDx12::PureMesh<RDx12::VertexData> GetPureMesh(
		FbxNode* fbxNode,
		std::vector<DirectX::XMFLOAT3>& vertexBoneWeight,
		std::vector<DirectX::XMUINT4>& vertexBoneIdx,
		const std::vector<DirectX::XMFLOAT3>& controlPointBoneWeight,
		const std::vector<DirectX::XMUINT4>& controlPointBoneIdx
	);
	static void GetTransMidData(RFbxNode* rNode, FbxObject* pObject);
	static std::vector<std::vector<RDx12::AnimCurve>> GetAnimMidData(FbxNode* lNode, FbxScene* lScene);
	static std::vector<std::vector<RFbxLink>> GetSkinMidData(FbxNode* fbxNode);
private:
	static RDx12::VertexData FbxVertex2DxVertex(RDx12::VertexData fbxVt);
};
