#pragma once
#include <RDx12/Geomertry/Skeleton.h>
#include <RDx12/Geomertry/PureMesh.h>
#include "SceneNode.h"

class Scene
{
public:
	std::shared_ptr<SceneNode> root;
	std::vector<RDx12::PureMesh<RDx12::VertexData>> vecPureMesh;
	std::vector<RDx12::PureMesh<RDx12::SkinVertexData>> vecSkinMesh;
	std::vector<DirectX::XMFLOAT4X4> vecSkinMeshBindWorldMat;
	std::vector<RDx12::Skeleton> vecSkeleton;
	Scene(){}
	void RSerialize_Save(std::ofstream& outFile);
	void RSerialize_Load(std::ifstream& inFile);
	static bool AddChild(std::shared_ptr<SceneNode> father, std::shared_ptr<SceneNode> child);
	static bool AddChildKeepWorldMat(std::shared_ptr<SceneNode> father, std::shared_ptr<SceneNode> child);
	static bool RemoveNode(std::shared_ptr<SceneNode> nd);
	static void DoForAllChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func);
	static void DoForNdAndAllChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func);
	static void DoForLayerChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func);
	static void DoForNdAndLayerChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func);
	static std::vector<std::shared_ptr<SceneNode>> GetNodeVec(std::shared_ptr<SceneNode> nd, std::function<bool(std::shared_ptr<SceneNode>)> func);
	static void ShowHierarchy(std::shared_ptr<SceneNode> A,int depth);
	static int IsSameRoute(std::shared_ptr<SceneNode> nd1, std::shared_ptr<SceneNode> nd2,std::shared_ptr<SceneNode> root);
	static void IsSameRouteRcs(int& timeCount,int& inTimeNd1,int& outTimeNd1,int& inTimeNd2,int& outTimeNd2,std::shared_ptr<SceneNode> nd1, std::shared_ptr<SceneNode> nd2, std::shared_ptr<SceneNode> nowNd);
};
