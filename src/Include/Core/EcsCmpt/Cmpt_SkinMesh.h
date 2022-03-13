#pragma once
#include <RDx12/Geomertry/Skeleton.h>
#include "../../Core/ShaderDesign/ShaderManager.h"
#include "Cmpt_Trans.h"


struct Cmpt_SkinMesh
{
	int mSkinMeshIdx = 0;int mSkinSubMeshIdx = 0;
	RDx12::FrameResAlloc_RootCb<SkinnedCb> mSkinnedCb;
	
	//from Cmpt_SkinRoot
	std::vector<DirectX::XMFLOAT4X4>* mOffsetMulAnimWorld;
	DirectX::XMFLOAT4X4 mSkinRootWorldMat = RDx12::MathHelper::Identity4x4();

	static std::function<void(Cmpt_SkinMesh*)> Update(RDx12::FrVec& fv, ID3D12Device4* dev, std::vector<DirectX::XMFLOAT4X4>& vecSkinMeshBindWorldMat);
	
	void AllocRes(RDx12::Sign_RootCb<SkinnedCb>& sd_SkinnedCb);
	void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv);

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};
