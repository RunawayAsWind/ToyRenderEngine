#pragma once
#include "Cmpt_SkinRoot.h"
#include "Cmpt_Trans.h"


struct AnimUnit
{
	std::string clipName;
	DirectX::XMFLOAT3 posOffset = { 0, 0, 0 };
	DirectX::XMFLOAT3 scaleOffset = { 1,1,1 };
	DirectX::XMFLOAT4 quatOffset = { 0, 0, 0, 1 };

	void RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteStr(outFile, clipName);
		RWin32::Bin_WriteData(outFile, posOffset);
		RWin32::Bin_WriteData(outFile, scaleOffset);
		RWin32::Bin_WriteData(outFile, quatOffset);
	}

	void RSerialize_Load(std::ifstream& inFile)
	{
		clipName = RWin32::Bin_LoadStr(inFile);
		posOffset = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
		scaleOffset = RWin32::Bin_LoadData<DirectX::XMFLOAT3>(inFile);
		quatOffset = RWin32::Bin_LoadData<DirectX::XMFLOAT4>(inFile);
	}
};

struct AnimLogic
{
	std::string name;
	std::vector<AnimUnit> vecAnimUint;

	void RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteStr(outFile, name);
		RWin32::Bin_WriteSerializeVec(outFile, vecAnimUint);
	}

	void RSerialize_Load(std::ifstream& inFile)
	{
		name = RWin32::Bin_LoadStr(inFile);
		vecAnimUint = RWin32::Bin_LoadSerializeVec<AnimUnit>(inFile);
	}
};

struct Cmpt_CmdParser
{
	std::vector<AnimLogic> mLogicVec{ {"DefaultLogic",{{"Default"}}} };
	int mLogicIdx = 0;
	int mUnitIdx = 0;
	float mTimePos = 0;
	float mSpeed = 1;

	static std::function<void(Cmpt_CmdParser*,Cmpt_SkinRoot*, Cmpt_Trans*)> Update()
	{
		return std::function
		(
			[](Cmpt_CmdParser *cmdParser, Cmpt_SkinRoot* skinRoot, Cmpt_Trans* tr)
			{
				if (skinRoot->mTimePos >= skinRoot->mClipVec[skinRoot->mClipIdx].endTime)
				{
				
					if (cmdParser->mUnitIdx < cmdParser->GetAnimUnitList().size()-1)
					{
						cmdParser->mUnitIdx++;
					}
					else
					{
						cmdParser->mUnitIdx = 0;
					}
					
					std::string clipName = cmdParser->GetAnimUnit().clipName;
					int findIdx = -1;
					for (int i=0;i<skinRoot->mClipVec.size();i++)
					{
						RDx12::AnimClip animClip = skinRoot->mClipVec[i];
						std::cout << animClip.name.size()<<" "<< clipName.size() << std::endl;
						std::cout << animClip.name << " " << clipName << std::endl;
						if (animClip.name == clipName)
						{
							std::cout << animClip.name << " " << clipName << std::endl;
							findIdx = i;
							break;
						}
					}
						
					if (findIdx != -1)
					{
						skinRoot->ChangeClip(findIdx);
					}
					else
					{
						cmdParser->mUnitIdx++;
						if (cmdParser->mUnitIdx < cmdParser->GetAnimUnitList().size() - 1)
						{
							cmdParser->mUnitIdx++;
						}
						else
						{
							cmdParser->mUnitIdx = 0;
						}
					}
				}
			}
		);
	}

	std::vector<AnimUnit>& GetAnimUnitList()
	{
		return mLogicVec[mLogicIdx].vecAnimUint;
	}

	AnimUnit& GetAnimUnit()
	{
		return mLogicVec[mLogicIdx].vecAnimUint[mUnitIdx];
	}

	void RSerialize_Save(std::ofstream& outFile)
	{
		RWin32::Bin_WriteSerializeVec(outFile, mLogicVec);
		RWin32::Bin_WriteData(outFile, mLogicIdx);
		RWin32::Bin_WriteData(outFile, mSpeed);
	}

	void RSerialize_Load(std::ifstream& inFile)
	{
		mLogicVec = RWin32::Bin_LoadSerializeVec<AnimLogic>(inFile);
		mLogicIdx = RWin32::Bin_LoadData<int>(inFile);
		mSpeed = RWin32::Bin_LoadData<float>(inFile);
	}
};
