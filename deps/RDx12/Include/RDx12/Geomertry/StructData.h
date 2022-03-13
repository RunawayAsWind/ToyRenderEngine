#pragma once
#include "PureMesh.h"
#include "../ResCreate/Res/Buffer/Res_DefaultSingleTypeBuffer.h"

namespace RDx12
{
	template<typename vType>
	struct Struct_MeshMerge
	{
		std::vector<vType> vecVertex;
		std::vector<IndexType> vecIndex;
		std::vector<SubMesh> vecSubMesh;
		
		void AddMesh(PureMesh<vType> newMesh)
		{
			SubMesh newSub;
			newSub.name = newMesh.name;
			newSub.indexCount = newMesh.vecIndex.size();
			newSub.startVertexLocation = vecVertex.size();
			newSub.startIndexLocation = vecIndex.size();
			newSub.topology = newMesh.topology;
			vecSubMesh.emplace_back(newSub);

			vecVertex.insert(vecVertex.end(), std::begin(newMesh.vecVertex), std::end(newMesh.vecVertex));
			vecIndex.insert(vecIndex.end(), std::begin(newMesh.vecIndex), std::end(newMesh.vecIndex));
		}

		void ClearAll()
		{
			vecVertex.clear();
			vecIndex.clear();
			vecSubMesh.clear();
		}

		void Struct_MeshMerge::ShowInfo()
		{
			WIN32DEBUG(L"\n");
			WIN32DEBUG(L"----------StructData.h Struct_MeshMerge ShowInfo----------\n");
			WIN32DEBUG(L"Vertex:%d\n", vecVertex.size());
			WIN32DEBUG(L"Index:%d\n", vecIndex.size());
			WIN32DEBUG(L"SubMesh:\n");
			for (int i = 0; i < vecSubMesh.size(); i++)
			{
				SubMesh subMesh = vecSubMesh[i];
				WIN32DEBUG(L"MeshName:%s  IndexCount:%d   StartIndexLocation:%d  StartVertexLocation:%d  Topology:%d\n", RWin32::Win32_Str2Ws(subMesh.name).c_str(), subMesh.indexCount, subMesh.startIndexLocation, subMesh.startVertexLocation, subMesh.topology);
			}
			WIN32DEBUG(L"\n");
		}
	};

	template<typename vType>
	class Struct_MeshRes
	{
	public:
		Struct_MeshRes(BufferType resType)
		{
			res_Vertex = GenSingleTypeBuffer(sizeof(vType), false, resType);
			res_Index = GenSingleTypeBuffer(sizeof(IndexType), false, resType);
		}

		void CreateByMaxNum(UINT vertexNum, UINT indexNum, ID3D12Device4* dev)
		{
			stMeshMerge.ClearAll();
			res_Vertex->CreateByMaxNum(vertexNum, dev); res_Vertex->SetMap();
			res_Index->CreateByMaxNum(indexNum, dev); res_Index->SetMap();
		}

		void AddMesh(PureMesh<vType> newMesh)
		{
			stMeshMerge.AddMesh(newMesh);
		}

		void AddMesh(PureMesh<vType> newMesh, ID3D12GraphicsCommandList* cmdList)
		{
			stMeshMerge.AddMesh(newMesh);
			UpdateRes(cmdList);
		}

		void UpdateRes(ID3D12GraphicsCommandList* cmdList)
		{
			if (stMeshMerge.vecIndex.size() > 0 && stMeshMerge.vecVertex.size() > 0)
			{
				for (int i = 0; i < stMeshMerge.vecVertex.size(); i++)
					res_Vertex->CopyData(i, &stMeshMerge.vecVertex[i]);
				res_Vertex->UpdateRes(cmdList);

				for (int i = 0; i < stMeshMerge.vecIndex.size(); i++)
					res_Index->CopyData(i, &stMeshMerge.vecIndex[i]);
				res_Index->UpdateRes(cmdList);
			}
		}

		SubMesh GetSubMesh(size_t idx) { return stMeshMerge.vecSubMesh[idx]; }
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() { return res_Index->IndexBufferView(); }
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() { return res_Vertex->VertexBufferView(); }
		void ShowInfo() { stMeshMerge.ShowInfo(); }
	private:
		Struct_MeshMerge<vType> stMeshMerge;
		std::shared_ptr<Res_SingleTypeBuffer> res_Vertex;
		std::shared_ptr<Res_SingleTypeBuffer> res_Index;
	};
}