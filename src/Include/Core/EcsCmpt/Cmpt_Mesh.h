#pragma once
#include<RWin32/FileHandler.h>


/*
Scene.vecPureMesh[A,B,C,D]
 
���������ڵ���������ԣ�mMeshIdx[3,0,1,1] (D,A,B,B)

����Scene���нڵ��mMeshIdx������Res��MeshRes.SubMesh[D,A,B] 

�������нڵ��mSubMeshIdx[0,1,2,2]

mSubMeshIdx����SetVertex Index��������Ⱦ
*/

struct Cmpt_Mesh
{
	int mMeshIdx = 0;int mSubMeshIdx = 0;

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};
