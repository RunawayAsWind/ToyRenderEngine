#include "../../../Include/Core/EcsCmpt/Cmpt_ObjCb.h"
#include <RWin32/FileHandler.h>

std::function<void(Cmpt_ObjCb*, Cmpt_Trans*)> Cmpt_ObjCb::Update(RDx12::FrVec& fv,ID3D12Device4* dev)
{
	return std::function
	(
		[&fv,dev](Cmpt_ObjCb* cb,Cmpt_Trans* tr)
		{	
            if (cb->mDirty || tr->mDirty)
            {
				cb->mDirty = false;
                cb->mFrameDirty = fv.frResVec.size();
            }
            if (cb->mFrameDirty > 0)
            {
                cb->mObjCb.data = GetObjCb(
                    DirectX::XMLoadFloat4x4(&tr->mWorldMat),
                    DirectX::XMLoadFloat4x4(&cb->mObjCb.data.gTexTransform),
                    cb->mObjCb.data.gRenderIdx,
					cb->mObjCb.data.gObjTexCbIdx,
					cb->mObjCb.data.gIsSelected,
					cb->mObjCb.data.gPadding2
                );
                cb->mObjCb.Update(fv.GetNowFrIdx());
                cb->mFrameDirty--;
            }
		}
	);
}

void Cmpt_ObjCb::MustDirty()
{
    mDirty = true;
}

void Cmpt_ObjCb::AllocRes(RDx12::Sign_RootCb<ObjCb>& sd_ObjCb)
{
    mObjCb = sd_ObjCb.Get();
}

void Cmpt_ObjCb::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv)
{
    mObjCb.SetGraphicsRootConstantBufferView(fv.GetNowFrIdx(), cl);
}

void Cmpt_ObjCb::RSerialize_Save(std::ofstream& outFile)
{
    RWin32::Bin_WriteData(outFile, mObjCb.data.gRenderIdx);
    RWin32::Bin_WriteData(outFile, mObjCb.data.gObjTexCbIdx);
}

void Cmpt_ObjCb::RSerialize_Load(std::ifstream& inFile)
{
    mObjCb.data.gRenderIdx = RWin32::Bin_LoadData<int>(inFile);
    mObjCb.data.gObjTexCbIdx = RWin32::Bin_LoadData<int>(inFile);
}