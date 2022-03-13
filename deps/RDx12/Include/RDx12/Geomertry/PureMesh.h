#pragma once
#include <d3d12.h>
#include <RWin32/FileHandler.h>
#include "../Util/D12MathUtil.h"

namespace RDx12
{
	using IndexType = uint32_t;

	struct VertexData
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(0, 0, 0, 0);
		DirectX::XMFLOAT3 nor = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT2 texc = DirectX::XMFLOAT2(0, 0);
		DirectX::XMFLOAT3 tan = DirectX::XMFLOAT3(0, 0, 0);
	};

	struct SkinVertexData
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(0, 0, 0, 0);
		DirectX::XMFLOAT3 nor = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT2 texc = DirectX::XMFLOAT2(0, 0);
		DirectX::XMFLOAT3 tan = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT3 boneWeight = DirectX::XMFLOAT3(-1, -1, -1);
		DirectX::XMUINT4 boneIdx = DirectX::XMUINT4(-1, -1, -1, -1);
	};

	template<typename vType>
	struct PureMesh
	{
		std::string name;
		std::vector<vType> vecVertex;
		std::vector<IndexType> vecIndex;
		D3D_PRIMITIVE_TOPOLOGY topology;

		void CalcuTangent()
		{
			for (int i = 0; i < vecIndex.size(); i += 3)
			{
				DirectX::XMFLOAT3 tan, biNorm;
				RDx12::MathHelper::ComputeTanAndBiNorm(
					tan, biNorm,
					vecVertex[vecIndex[i]].pos, vecVertex[vecIndex[i + 1]].pos, vecVertex[vecIndex[i + 2]].pos,
					vecVertex[vecIndex[i]].texc, vecVertex[vecIndex[i + 1]].texc, vecVertex[vecIndex[i + 2]].texc
				);
				vecVertex[vecIndex[i]].tan = tan;
				vecVertex[vecIndex[i + 1]].tan = tan;
				vecVertex[vecIndex[i + 2]].tan = tan;
			}
		}

		void RSerialize_Save(std::ofstream& outFile)
		{
			RWin32::Bin_WriteStr(outFile, name);
			RWin32::Bin_WriteVec(outFile, vecVertex);
			RWin32::Bin_WriteVec(outFile, vecIndex);
			RWin32::Bin_WriteData(outFile, topology);
		}

		void RSerialize_Load(std::ifstream& inFile)
		{
			name = RWin32::Bin_LoadStr(inFile);
			vecVertex = RWin32::Bin_LoadVec<vType>(inFile);
			vecIndex = RWin32::Bin_LoadVec<IndexType>(inFile);
			topology = RWin32::Bin_LoadData<D3D_PRIMITIVE_TOPOLOGY>(inFile);
		}
	};

	struct SubMesh
	{
		std::string name;
		UINT indexCount = 0;
		UINT startIndexLocation = 0;
		UINT startVertexLocation = 0;
		D3D_PRIMITIVE_TOPOLOGY topology;

		void RSerialize_Save(std::ofstream& outFile)
		{
			RWin32::Bin_WriteStr(outFile, name);
			RWin32::Bin_WriteData(outFile, indexCount);
			RWin32::Bin_WriteData(outFile, startIndexLocation);
			RWin32::Bin_WriteData(outFile, startVertexLocation);
			RWin32::Bin_WriteData(outFile, topology);
		}

		void RSerialize_Load(std::ifstream& inFile)
		{
			name = RWin32::Bin_LoadStr(inFile);
			indexCount = RWin32::Bin_LoadData<UINT>(inFile);
			startIndexLocation = RWin32::Bin_LoadData<UINT>(inFile);
			startVertexLocation = RWin32::Bin_LoadData<UINT>(inFile);
			topology = RWin32::Bin_LoadData<D3D_PRIMITIVE_TOPOLOGY>(inFile);
		}
	};

	static class GeoGen
	{
	public:
		static PureMesh<VertexData> GetPlane(std::string&& name, float x2, float y2);
		static PureMesh<VertexData> GetCube(std::string&& name, float x2, float y2, float z2);
		static PureMesh<VertexData> GetSphere(std::string&& name, float radius, uint32_t sliceCount, uint32_t stackCount);
	};
}
