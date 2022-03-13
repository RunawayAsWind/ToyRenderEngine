#include "../../../Include/Core/EcsCmpt/Cmpt_Trans.h"
#include <RWin32/FileHandler.h>

std::function<void(Cmpt_Trans*)> Cmpt_Trans::End()
{
	return std::function
	(
		[](Cmpt_Trans* tr)
		{	
			tr->mDirty = false;
		}
	);
}

void Cmpt_Trans::MustDirty()
{
	mDirty = true;
}

void Cmpt_Trans::ComputeGobalTrans(Cmpt_Trans* fatherTrans)
{
	if (fatherTrans != nullptr)
	{
		mWorldMat = RDx12::MathHelper::MulMat(GetOffsetMat(), fatherTrans->mWorldMat);
		DirectX::XMStoreFloat4(&mQuat, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&mOffsetQuat), DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&fatherTrans->mQuat))));
	}
	else
	{
		mWorldMat = GetOffsetMat();
		mQuat = mOffsetQuat;
	}
	mPos = RDx12::MathHelper::F3MulMat({0,0,0}, mWorldMat);
	mEuler = RDx12::MathHelper::Quat2EluerDeg(mQuat);
	mForward = RDx12::MathHelper::TransNormal(RDx12::MathHelper::Foward(), mWorldMat);
	mRight = RDx12::MathHelper::TransNormal(RDx12::MathHelper::Right(), mWorldMat);
	mUp = RDx12::MathHelper::TransNormal(RDx12::MathHelper::Up(), mWorldMat);
	mDirty = true;
}

DirectX::XMFLOAT4X4 Cmpt_Trans::GetOffsetMat()
{
	DirectX::XMFLOAT4X4 SRTMat;
	DirectX::XMStoreFloat4x4(
		&SRTMat,
		RDx12::MathHelper::GetTransMatByF(
			{ 0,0,0 },
			{ 0,0,0,1 },
			mOffsetScale,
			{ 0,0,0 },
			mOffsetQuat,
			mOffsetPos
		)
	);
	DirectX::XMFLOAT4X4 offsetMat = RDx12::MathHelper::MulMat(mKeepWorldMat, SRTMat);
	return offsetMat;
}

void Cmpt_Trans::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteData(outFile, mOffsetPos);
	RWin32::Bin_WriteData(outFile, mOffsetScale);
	RWin32::Bin_WriteData(outFile, mOffsetQuat);
	RWin32::Bin_WriteData(outFile, mKeepWorldMat);
}

void Cmpt_Trans::RSerialize_Load(std::ifstream& inFile)
{
	mOffsetPos = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
	mOffsetScale = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
	mOffsetQuat = RWin32::Bin_LoadData<DirectX::XMFLOAT4>(inFile);
	mKeepWorldMat = RWin32::Bin_LoadData<DirectX::XMFLOAT4X4>(inFile);
}