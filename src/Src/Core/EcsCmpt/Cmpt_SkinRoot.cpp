#include "../../../Include/Core/EcsCmpt/Cmpt_SkinRoot.h"
#include <RWin32/FileHandler.h>

std::function<void(Cmpt_SkinRoot*)> Cmpt_SkinRoot::Update(RWin32::GameTimer* gameTimer, std::vector<RDx12::Skeleton>& vecSkeleton)
{
	return std::function
	(
		[gameTimer, &vecSkeleton](Cmpt_SkinRoot* sr)
		{
			if (sr->mSkeletonIdx < vecSkeleton.size())
			{
				
				for (int i=0;i<vecSkeleton[sr->mSkeletonIdx].mNameVec.size();i++)
				{
					std::string name = vecSkeleton[sr->mSkeletonIdx].mNameVec[i];
					if (sr->mInputFrameInter.find(name)!=sr->mInputFrameInter.end())
					{
						DirectX::XMStoreFloat4(
							&sr->mInputFrame[name].mQuat,
							DirectX::XMQuaternionSlerp(
								DirectX::XMLoadFloat4(&sr->mInputFrame[name].mQuat),
								DirectX::XMLoadFloat4(&sr->mInputFrameInter[name].mQuat),
								0.05f)
						);
					}
				}

				
				if (sr->mTimePos > sr->mClipVec[sr->mClipIdx].endTime)
				{
					sr->mTimePos = sr->mClipVec[sr->mClipIdx].startTime;
				}
				else
				{
					sr->mTimePos += gameTimer->DeltaTime() * sr->mSpeed;
				}

				sr->mOffsetMulAnimWorld = vecSkeleton[sr->mSkeletonIdx].GetOffsetMulAnimWorld(sr->mTimePos, &sr->mInputFrame);
			}
		}
	);
}

void Cmpt_SkinRoot::ChangeClip(int idx)
{
	mClipIdx = idx;
	mTimePos = mClipVec[idx].startTime;
}

void Cmpt_SkinRoot::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteSerializeVec(outFile, mClipVec);
	RWin32::Bin_WriteData(outFile, mClipIdx);
	RWin32::Bin_WriteData(outFile, mSpeed);
}

void Cmpt_SkinRoot::RSerialize_Load(std::ifstream& inFile)
{
	mClipVec = RWin32::Bin_LoadSerializeVec<RDx12::AnimClip>(inFile);
	mClipIdx = RWin32::Bin_LoadData<int>(inFile);
	mSpeed = RWin32::Bin_LoadData<float>(inFile);
	ChangeClip(mClipIdx);
}
