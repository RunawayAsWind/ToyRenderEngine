#include "../../../Include/Core/EcsCmpt/Cmpt_PassCb.h"

std::function<void(Cmpt_PassCb*, Cmpt_Camera*, Cmpt_Trans*)> Cmpt_PassCb::Update(RDx12::FrVec& fv)
{
    return std::function
	(
		[&fv](Cmpt_PassCb* cb, Cmpt_Camera* cam, Cmpt_Trans* tr)
		{	
            cb->mPassCb.data = GetPassCb(
                tr->mPos,
                DirectX::XMLoadFloat4x4(&cam->mViewMat),
                DirectX::XMLoadFloat4x4(&cam->mProjMat),
                cb->mPassCb.data.gRenderTargetSize,
                cam->mNearSize,
                cam->mNearZ,
                cam->mFarZ,
                cb->mPassCb.data.gTotalTime,
                cb->mPassCb.data.gDeltaTime
            );
            cb->mPassCb.Update(fv.GetNowFrIdx());
		}
	);
}

void Cmpt_PassCb::AllocRes(RDx12::Sign_RootCb<PassCb>& sd_PassCb)
{
    mPassCb = sd_PassCb.Get();
}

void Cmpt_PassCb::PreUpdate(DirectX::XMFLOAT2 RenderTargetSize, DirectX::XMFLOAT2 InvRenderTargetSize,float totalTime,float deltaTime)
{
    mPassCb.data.gRenderTargetSize = RenderTargetSize;
    mPassCb.data.gInvRenderTargetSize = InvRenderTargetSize;
    mPassCb.data.gTotalTime = totalTime;
    mPassCb.data.gDeltaTime = deltaTime;
}

void Cmpt_PassCb::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv)
{
    mPassCb.SetGraphicsRootConstantBufferView(fv.GetNowFrIdx(), cl);
}

void Cmpt_PassCb::SetComputeRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv)
{
	mPassCb.SetComputeRootConstantBufferView(fv.GetNowFrIdx(), cl);
}
