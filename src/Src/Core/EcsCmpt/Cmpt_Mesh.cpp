#include "../../../Include/Core/EcsCmpt/Cmpt_Mesh.h"

void Cmpt_Mesh::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteData(outFile, mMeshIdx);
}

void Cmpt_Mesh::RSerialize_Load(std::ifstream& inFile)
{
	mMeshIdx = RWin32::Bin_LoadData<int>(inFile);
}