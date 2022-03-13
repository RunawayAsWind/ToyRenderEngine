#pragma once
#include "Cmpt_Trans.h"

struct Cmpt_Camera
{
	float mNearZ = 0;
	float mFarZ = 0;
	float mAspect = 0;		
	float mFovVerDeg = 0;

	float mFovHorDeg = 0;
	DirectX::XMFLOAT2 mNearSize = { 0,0 };
	DirectX::XMFLOAT2 mFarSize = { 0,0 };
	DirectX::XMFLOAT4X4 mViewMat = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProjMat = RDx12::MathHelper::Identity4x4();

	static std::function<void(Cmpt_Camera*, Cmpt_Trans*)> Update();
	void SetFrustum(float nearZ, float farZ, float aspect, float fovVerDeg);

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};

template<>
struct RTmpl::Reflection<Cmpt_Camera>
{
	static constexpr RTmpl::FieldList valList = {
		RTmpl::Field{"Near",&Cmpt_Camera::mNearZ},
		RTmpl::Field{"Far",&Cmpt_Camera::mFarZ},
		RTmpl::Field{"FovVerDeg",&Cmpt_Camera::mFovVerDeg}
	};

	template<typename Func>
	static void DoForEachVal(Func func)
	{
		RTmpl::TransTupleDo(valList.tp, func);
	}
};
