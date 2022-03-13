#include "../../../Include/Core/EcsCmpt/Cmpt_Camera.h"
#include <RWin32/FileHandler.h>

std::function<void(Cmpt_Camera*, Cmpt_Trans*)> Cmpt_Camera::Update()
{
	return std::function
	(
		[](Cmpt_Camera* cam, Cmpt_Trans* tr)
		{
			if (tr->mDirty)
			{
				DirectX::XMVECTOR focuspos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&tr->mPos), DirectX::XMLoadFloat3(&tr->mForward));
				DirectX::XMStoreFloat4x4(&cam->mViewMat, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&tr->mPos), focuspos, DirectX::XMLoadFloat3(&tr->mUp)));
			}
		}
	);
}

void Cmpt_Camera::SetFrustum(float nearZ, float farZ, float aspect, float fovVerDeg)
{
	mNearZ = nearZ;
	mFarZ = farZ;
	mAspect = aspect;
	mFovVerDeg = fovVerDeg;
	float FovVerRad = DirectX::XMConvertToRadians(fovVerDeg);
	float FovHorRad = std::atan(std::tan(FovVerRad / 2) * aspect)*2;
	mFovHorDeg = DirectX::XMConvertToDegrees(FovHorRad);
	mFarSize.x = farZ * std::tan(FovHorRad / 2) * 2;
	mFarSize.y = farZ * std::tan(FovVerRad / 2) * 2;
	mNearSize.x = nearZ * std::tan(FovHorRad / 2) * 2;
	mNearSize.y = nearZ * std::tan(FovVerRad / 2) * 2;

	DirectX::XMStoreFloat4x4(&mProjMat, DirectX::XMMatrixPerspectiveFovLH(FovVerRad, aspect, nearZ, farZ));
}


void Cmpt_Camera::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteData(outFile, mNearZ);
	RWin32::Bin_WriteData(outFile, mFarZ);
	RWin32::Bin_WriteData(outFile, mAspect);
	RWin32::Bin_WriteData(outFile, mFovVerDeg);
}

void Cmpt_Camera::RSerialize_Load(std::ifstream& inFile)
{
	mNearZ = RWin32::Bin_LoadData<float>(inFile);
	mFarZ = RWin32::Bin_LoadData<float>(inFile);
	mAspect = RWin32::Bin_LoadData<float>(inFile);
	mFovVerDeg = RWin32::Bin_LoadData<float>(inFile);
	SetFrustum(mNearZ, mFarZ, mAspect, mFovVerDeg);
}
