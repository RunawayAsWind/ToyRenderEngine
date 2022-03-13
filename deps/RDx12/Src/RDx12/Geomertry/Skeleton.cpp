#include "../../../Include/RDx12/Geomertry/Skeleton.h"

namespace RDx12
{
	void AnimClip::RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteStr(outFile, name);
		RWin32::Bin_WriteData(outFile, startTime);
		RWin32::Bin_WriteData(outFile, endTime);
	}

	void AnimClip::RSerialize_Load(std::ifstream& inFile)
	{
		name = RWin32::Bin_LoadStr(inFile);
		startTime = RWin32::Bin_LoadData<float>(inFile);
		endTime = RWin32::Bin_LoadData<float>(inFile);
	}

	DirectX::XMFLOAT4X4 KeyFrame::LerpMat(KeyFrame f1, KeyFrame f2, float lerpPercent)
	{
		DirectX::XMFLOAT4X4 mat = RDx12::MathHelper::Identity4x4();
		DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&f1.mScale);
		DirectX::XMVECTOR s2 = DirectX::XMLoadFloat3(&f2.mScale);

		DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&f1.mTrans);
		DirectX::XMVECTOR t2 = DirectX::XMLoadFloat3(&f2.mTrans);

		DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&f1.mQuat);
		DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&f2.mQuat);

		DirectX::XMVECTOR S = DirectX::XMVectorLerp(s1, s2, lerpPercent);
		DirectX::XMVECTOR T = DirectX::XMVectorLerp(t1, t2, lerpPercent);
		DirectX::XMVECTOR Q = DirectX::XMQuaternionSlerp(q1, q2, lerpPercent);

		DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixAffineTransformation(S, zero, Q, T));
		return mat;
	}

	DirectX::XMFLOAT3 KeyFrame::LerpTrans(KeyFrame f1, KeyFrame f2, float lerpPercent)
	{
		DirectX::XMFLOAT3 trans(0, 0, 0);
		DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&f1.mTrans);
		DirectX::XMVECTOR t2 = DirectX::XMLoadFloat3(&f2.mTrans);
		DirectX::XMVECTOR T = DirectX::XMVectorLerp(t1, t2, lerpPercent);
		DirectX::XMStoreFloat3(&trans, T);
		return trans;
	}

	DirectX::XMFLOAT4 KeyFrame::LerpQuat(KeyFrame f1, KeyFrame f2, float lerpPercent)
	{
		DirectX::XMFLOAT4 quat(0, 0, 0, 1);
		DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&f1.mQuat);
		DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&f2.mQuat);
		DirectX::XMVECTOR Q = DirectX::XMQuaternionSlerp(q1, q2, lerpPercent);
		DirectX::XMStoreFloat4(&quat, Q);
		return quat;
	}

	DirectX::XMFLOAT3 KeyFrame::LerpScale(KeyFrame f1, KeyFrame f2, float lerpPercent)
	{
		DirectX::XMFLOAT3 scale(1, 1, 1);
		DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&f1.mScale);
		DirectX::XMVECTOR s2 = DirectX::XMLoadFloat3(&f2.mScale);
		DirectX::XMVECTOR S = DirectX::XMVectorLerp(s1, s2, lerpPercent);
		DirectX::XMStoreFloat3(&scale, S);
		return scale;
	}

	DirectX::XMFLOAT4X4 KeyFrame::GetMat()
	{
		DirectX::XMFLOAT4X4 mat = RDx12::MathHelper::Identity4x4();
		DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&mScale);
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&mTrans);
		DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&mQuat);
		DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixAffineTransformation(S, zero, Q, P));
		return mat;
	}

	void KeyFrame::RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteData(outFile, TimePos);
		RWin32::Bin_WriteData(outFile, mTrans);
		RWin32::Bin_WriteData(outFile, mScale);
		RWin32::Bin_WriteData(outFile, mQuat);
	}

	void KeyFrame::RSerialize_Load(std::ifstream& inFile)
	{
		TimePos = RWin32::Bin_LoadData<float>(inFile);
		mTrans = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
		mScale = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
		mQuat = RWin32::Bin_LoadData<DirectX::XMFLOAT4>(inFile);
	}

	void AnimCurve::FindFrontAndBack(float t, int& front, int& back)
	{
		if (mFrameVec.size() != 0)
		{
			if (t <= mFrameVec.front().TimePos)
			{
				front = 0;
				back = 0;
			}
			else if (t >= mFrameVec.back().TimePos)
			{
				front = mFrameVec.size() - 1;
				back = mFrameVec.size() - 1;
			}
			else
			{
				for (UINT i = 0; i < mFrameVec.size() - 1; ++i)
				{
					if (t >= mFrameVec[i].TimePos && t <= mFrameVec[i + 1].TimePos)
					{
						front = i;
						back = i + 1;
						break;
					}
				}
			}
		}
	}

	DirectX::XMFLOAT4X4 AnimCurve::LerpMat(float t)
	{
		DirectX::XMFLOAT4X4 mat = RDx12::MathHelper::Identity4x4();
		int front, back;
		front = back = -1;
		FindFrontAndBack(t, front, back);
		if (front != -1 && back != -1)
		{
			if (front == back)
			{
				mat = mFrameVec[front].GetMat();
			}
			else
			{
				float lerpPercent = (t - mFrameVec[front].TimePos) / (mFrameVec[back].TimePos - mFrameVec[front].TimePos);
				mat = KeyFrame::LerpMat(mFrameVec[front], mFrameVec[back], lerpPercent);
			}
		}
		return mat;
	}

	DirectX::XMFLOAT3 AnimCurve::LerpTrans(float t)
	{
		DirectX::XMFLOAT3 trans(0, 0, 0);
		int front, back;
		front = back = -1;
		FindFrontAndBack(t, front, back);
		if (front != -1 && back != -1)
		{
			if (front == back)
			{
				trans = mFrameVec[front].mTrans;
			}
			else
			{
				float lerpPercent = (t - mFrameVec[front].TimePos) / (mFrameVec[back].TimePos - mFrameVec[front].TimePos);
				trans = KeyFrame::LerpTrans(mFrameVec[front], mFrameVec[back], lerpPercent);
			}
		}
		return trans;
	}

	DirectX::XMFLOAT4 AnimCurve::LerpQuat(float t)
	{
		DirectX::XMFLOAT4 quat(0, 0, 0, 1);
		int front, back;
		front = back = -1;
		FindFrontAndBack(t, front, back);
		if (front != -1 && back != -1)
		{
			if (front == back)
			{
				quat = mFrameVec[front].mQuat;
			}
			else
			{
				float lerpPercent = (t - mFrameVec[front].TimePos) / (mFrameVec[back].TimePos - mFrameVec[front].TimePos);
				quat = KeyFrame::LerpQuat(mFrameVec[front], mFrameVec[back], lerpPercent);
			}
		}
		return quat;
	}

	DirectX::XMFLOAT3 AnimCurve::LerpScale(float t)
	{
		DirectX::XMFLOAT3 scale(1, 1, 1);
		int front, back;
		front = back = -1;
		FindFrontAndBack(t, front, back);
		if (front != -1 && back != -1)
		{
			if (front == back)
			{
				scale = mFrameVec[front].mScale;
			}
			else
			{
				float lerpPercent = (t - mFrameVec[front].TimePos) / (mFrameVec[back].TimePos - mFrameVec[front].TimePos);
				scale = KeyFrame::LerpScale(mFrameVec[front], mFrameVec[back], lerpPercent);
			}
		}
		return scale;
	}

	void AnimCurve::RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteSerializeVec(outFile, mFrameVec);
	}

	void AnimCurve::RSerialize_Load(std::ifstream& inFile)
	{
		mFrameVec = RWin32::Bin_LoadSerializeVec<KeyFrame>(inFile);
	}

	std::vector<DirectX::XMFLOAT4X4> Skeleton::GetOffsetMulAnimWorld(float t, std::map<std::string, RDx12::KeyFrame>* mInputFrame)
	{
		std::vector<DirectX::XMFLOAT4X4> res;
		std::vector<DirectX::XMFLOAT4> quatRes;
		res.resize(mBoneVec.size());
		quatRes.resize(mBoneVec.size());

		int bipPelvisIdx = -1;
		//Get BoneWorldMat
		for (int i = 0; i < mBoneVec.size(); i++)
		{
			std::string name = mNameVec[i];
			DirectX::XMFLOAT4X4 parentMat = RDx12::MathHelper::Identity4x4();
			DirectX::XMFLOAT4 parentQuat{ 0,0,0,1 };
			if (mParentVec[i] != -1)
			{
				parentMat = res[mParentVec[i]];
				parentQuat = quatRes[mParentVec[i]];
			}
			res[i] = RDx12::MathHelper::MulMat(mBoneVec[i].LerpMat(t), parentMat);
			quatRes[i] = RDx12::MathHelper::QuatMulQuat(mBoneVec[i].LerpQuat(t), parentQuat);

			if (mInputFrame!=nullptr && mInputFrame->find(name)!=mInputFrame->end())
			{
				/*
				骨骼为正面旋转绕x旋转90度 
				骨骼到正面PrevilsQuat
				到正面后旋转90度 PrelvisQuat * 90mQuat = localQuat*parentQuat
				*/
				//DirectX::XMFLOAT4 goalWorldQuat = RDx12::MathHelper::QuatMulQuat(mBoneBindWorldQuat[mNameDic["Bip001 Pelvis"]], (*mInputFrame)[name].mQuat);
				
				/*
				(*mInputFrame)[name].mQuat 里记录的是相对绑定角度到估计角度的变换角度
				详见RecogWidget.cpp
				*/
				DirectX::XMFLOAT4 goalWorldQuat = RDx12::MathHelper::QuatMulQuat(mBoneBindWorldQuat[mNameDic[name]], (*mInputFrame)[name].mQuat);

				RDx12::KeyFrame newLocalKf;
				newLocalKf.mTrans = mBoneVec[i].LerpTrans(t);
				newLocalKf.mQuat = RDx12::MathHelper::QuatMulQuat(goalWorldQuat, RDx12::MathHelper::QuatInver(parentQuat));
				newLocalKf.mScale = mBoneVec[i].LerpScale(t);
				
				res[i] = RDx12::MathHelper::MulMat(newLocalKf.GetMat(), parentMat);
				quatRes[i] = goalWorldQuat;
			}
		}

		//Mul(OffsetMat,BoneWorldMat) = 动画距离初始状态的相对变换 当time=0 Mul(OffsetMat,BoneWorldMat) = RDx12::MathHelper::Identity4x4()
		for (int i = 0; i < res.size(); i++)
		{
			res[i] = RDx12::MathHelper::MulMat(RDx12::MathHelper::InverMat(mBoneBindWorldMatVec[i]), res[i]);
		}
		return res;
	}
	
	void Skeleton::UpdateChildVec()
	{
		mChildVec.resize(mParentVec.size());
		for (int i = 0; i < mParentVec.size(); i++)
		{
			int parentIdx = mParentVec[i];
			if (parentIdx != -1)
			{
				mChildVec[parentIdx].push_back(i);
			}
			mNameDic[mNameVec[i]] = i;
		}
	}

	void Skeleton::RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteSerializeVec(outFile, mBoneVec);
		RWin32::Bin_WriteVec(outFile, mBoneBindWorldMatVec);
		RWin32::Bin_WriteVec(outFile, mBoneBindWorldQuat);
		RWin32::Bin_WriteVec(outFile, mParentVec);
		RWin32::Bin_WriteStrVec(outFile, mNameVec);
	}

	void Skeleton::RSerialize_Load(std::ifstream& inFile)
	{
		mBoneVec = RWin32::Bin_LoadSerializeVec<AnimCurve>(inFile);
		mBoneBindWorldMatVec = RWin32::Bin_LoadVec<DirectX::XMFLOAT4X4>(inFile);
		mBoneBindWorldQuat = RWin32::Bin_LoadVec<DirectX::XMFLOAT4>(inFile);
		mParentVec = RWin32::Bin_LoadVec<int>(inFile);
		mNameVec = RWin32::Bin_LoadStrVec(inFile);
		UpdateChildVec();
	}
}