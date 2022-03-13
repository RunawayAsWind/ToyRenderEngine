#pragma once
#include <RTmpl/Reflection.h>
#include <RDx12/Util/D12MathUtil.h>
#include <functional>

struct Cmpt_Trans
{
	DirectX::XMFLOAT3 mPos = { 0, 0, 0 };
	DirectX::XMFLOAT4 mQuat = { 0, 0, 0, 1 };
	DirectX::XMFLOAT3 mEuler = { 0, 0, 0 };

	DirectX::XMFLOAT3 mForward = RDx12::MathHelper::Foward();
	DirectX::XMFLOAT3 mRight = RDx12::MathHelper::Right();
	DirectX::XMFLOAT3 mUp = RDx12::MathHelper::Up();
	
	DirectX::XMFLOAT4X4 mWorldMat = RDx12::MathHelper::Identity4x4();
	DirectX::XMFLOAT3 mOffsetPos = { 0, 0, 0 };
	DirectX::XMFLOAT3 mOffsetScale = { 1, 1, 1 };
	DirectX::XMFLOAT4 mOffsetQuat = { 0, 0, 0, 1 };
	DirectX::XMFLOAT4X4 mKeepWorldMat = RDx12::MathHelper::Identity4x4();

	bool mDirty = true;

	static std::function<void(Cmpt_Trans*)> End();//由于Trans的dirty会影响其他物体，所以需要不能在Update里面将dirty置false，应该在worldUpdate最后false Dirty
	void MustDirty();
	
	void ComputeGobalTrans(Cmpt_Trans* fatherTrans);
	DirectX::XMFLOAT4X4 GetOffsetMat();

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};

template<>
struct RTmpl::Reflection<Cmpt_Trans>
{
	static constexpr RTmpl::FieldList valList = {
		RTmpl::Field{"Pos",&Cmpt_Trans::mPos},
		RTmpl::Field{"Euler",&Cmpt_Trans::mEuler},
		RTmpl::Field{"Scale",&Cmpt_Trans::mOffsetScale},
	};

	template<typename Func>
	static void DoForEachVal(Func func)
	{
		RTmpl::TransTupleDo(valList.tp, func);
	}
};

