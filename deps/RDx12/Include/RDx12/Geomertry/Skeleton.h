#pragma once
#include "../Util/D12MathUtil.h"
#include <RWin32/FileHandler.h>
#include<set>
#include<map>
namespace RDx12
{
	struct AnimClip
	{
		std::string name;
		float startTime;
		float endTime;
		void RSerialize_Save(std::ofstream& outFile);
		void RSerialize_Load(std::ifstream& inFile);
	};

	struct KeyFrame
	{
		float TimePos = 0;
		DirectX::XMFLOAT3 mTrans{0,0,0};
		DirectX::XMFLOAT3 mScale{1,1,1};
		DirectX::XMFLOAT4 mQuat{0,0,0,1};

		static DirectX::XMFLOAT4X4 LerpMat(KeyFrame f1, KeyFrame f2, float lerpPercent);
		static DirectX::XMFLOAT3 LerpTrans(KeyFrame f1, KeyFrame f2, float lerpPercent);
		static DirectX::XMFLOAT4 LerpQuat(KeyFrame f1, KeyFrame f2, float lerpPercent);
		static DirectX::XMFLOAT3 LerpScale(KeyFrame f1, KeyFrame f2, float lerpPercent);

		DirectX::XMFLOAT4X4 GetMat();
		void RSerialize_Save(std::ofstream& outFile);
		void RSerialize_Load(std::ifstream& inFile);
	};

	struct AnimCurve
	{
		std::vector<KeyFrame> mFrameVec;

		void FindFrontAndBack(float t, int& front, int& back);
		DirectX::XMFLOAT4X4 LerpMat(float t);
		DirectX::XMFLOAT3 LerpTrans(float t);
		DirectX::XMFLOAT4 LerpQuat(float t);
		DirectX::XMFLOAT3 LerpScale(float t);
		void RSerialize_Save(std::ofstream& outFile);
		void RSerialize_Load(std::ifstream& inFile);
	};

	struct Skeleton
	{
		std::vector<AnimCurve> mBoneVec;//各骨骼的局部坐标变换曲线
		std::vector<DirectX::XMFLOAT4X4> mBoneBindWorldMatVec;//记录各骨骼的绑定位姿(绑定时的世界矩阵)，其逆矩阵可将顶点转化到骨骼空间（逆矩阵又称为OffsetMat）
		std::vector<DirectX::XMFLOAT4> mBoneBindWorldQuat;
		std::vector<int> mParentVec;
		std::vector<std::string> mNameVec;
		std::vector<std::vector<int>> mChildVec;
		std::map<std::string, int> mNameDic;
		
		std::vector<DirectX::XMFLOAT4X4> GetOffsetMulAnimWorld(float t, std::map<std::string, RDx12::KeyFrame>* mInputFrame);
		void UpdateChildVec();
		void RSerialize_Save(std::ofstream& outFile);
		void RSerialize_Load(std::ifstream& inFile);
	};


}