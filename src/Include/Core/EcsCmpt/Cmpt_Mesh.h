#pragma once
#include<RWin32/FileHandler.h>


/*
Scene.vecPureMesh[A,B,C,D]
 
真正决定节点网格的属性：mMeshIdx[3,0,1,1] (D,A,B,B)

遍历Scene所有节点的mMeshIdx，生成Res：MeshRes.SubMesh[D,A,B] 

更新所有节点的mSubMeshIdx[0,1,2,2]

mSubMeshIdx用于SetVertex Index来进行渲染
*/

struct Cmpt_Mesh
{
	int mMeshIdx = 0;int mSubMeshIdx = 0;

	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
};
