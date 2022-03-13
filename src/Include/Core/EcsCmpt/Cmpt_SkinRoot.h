#pragma once
#include <RDx12/Geomertry/Skeleton.h>
#include <RWin32/GameTimer.h>
#include "Cmpt_ObjCb.h"
#include <map>
struct Cmpt_SkinRoot
{
	std::vector<RDx12::AnimClip> mClipVec{ {"Default",0,100} };
	int mClipIdx = 0;
	float mTimePos = 0;
	float mSpeed = 1;

	int mSkeletonIdx = 0;
	std::vector<DirectX::XMFLOAT4X4> mOffsetMulAnimWorld{ BoneMaxNum };
	std::map<std::string, RDx12::KeyFrame> mInputFrame;
	std::map<std::string, RDx12::KeyFrame> mInputFrameInter;
	static std::function<void(Cmpt_SkinRoot*)> Update(RWin32::GameTimer* gameTime,std::vector<RDx12::Skeleton>& vecSkeleton);
	void ChangeClip(int idx);
	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};
