#pragma once
#include "../../Core/ShaderDesign/ShaderManager.h"
#include "Cmpt_Trans.h"

struct Cmpt_ObjCb
{
	RDx12::FrameResAlloc_RootCb<ObjCb> mObjCb;
	int mFrameDirty = 0;
	bool mDirty = true;//ObjCb不影响其他组件，直接在Update内设置dirty为false
	
	static std::function<void(Cmpt_ObjCb*,Cmpt_Trans*)> Update(RDx12::FrVec& fv,ID3D12Device4* dev);
	void MustDirty();
	
	void AllocRes(RDx12::Sign_RootCb<ObjCb>& sd_ObjCb);
    void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv);

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};