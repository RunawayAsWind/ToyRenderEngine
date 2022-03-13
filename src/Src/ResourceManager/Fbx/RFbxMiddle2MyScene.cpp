#include "../../../Include/ResourceManage/Fbx/RFbxMiddle2MyScene.h"

void RFbxMiddle2MyScene::Add2NowScene(TestScene001_Base* testScene, RFbxMidData& midData)
{
	//将fbx场景Mesh读入当前场景
	int oldMeshNum = testScene->sc.vecPureMesh.size();
	std::cout << midData.mPureMeshVec.size() << std::endl;
	for (auto& pureMesh : midData.mPureMeshVec)
	{
		pureMesh.CalcuTangent();//计算mesh切线
		testScene->sc.vecPureMesh.emplace_back(pureMesh);
		std::cout << pureMesh.name << " " << pureMesh.vecVertex.size() << " " << pureMesh.vecIndex.size() << std::endl;
	}
	std::cout << std::endl;



	//将fbx场景SkinMesh读入当前场景
	int oldSkinMeshNum = testScene->sc.vecSkinMesh.size();
	std::cout << midData.mSkinMeshVec.size() << std::endl;
	
	for (int i=0;i<midData.mSkinMeshVec.size();i++)
	{
		auto skinMesh = midData.mSkinMeshVec[i];
		skinMesh.CalcuTangent();//计算mesh切线
		testScene->sc.vecSkinMesh.emplace_back(skinMesh);
		testScene->sc.vecSkinMeshBindWorldMat.emplace_back(midData.mSkinMeshBindWorldMat[i]);
		std::cout << skinMesh.name << " " << skinMesh.vecVertex.size() << " " << skinMesh.vecIndex.size() << std::endl;
		/*for (int i = 0; i < skinMesh.vecVertex.size(); i++)
		{
			WIN32DEBUG(L"pos: %.2f %.2f %.2f\n", skinMesh.vecVertex[i].pos.x, skinMesh.vecVertex[i].pos.y, skinMesh.vecVertex[i].pos.z);
			WIN32DEBUG(L"boneIdx: %d %d %d %d\n", skinMesh.vecVertex[i].boneIdx.x, skinMesh.vecVertex[i].boneIdx.y, skinMesh.vecVertex[i].boneIdx.z, skinMesh.vecVertex[i].boneIdx.w);
			WIN32DEBUG(L"weight: %.2f %.2f %.2f\n", skinMesh.vecVertex[i].boneWeight.x, skinMesh.vecVertex[i].boneWeight.y, skinMesh.vecVertex[i].boneWeight.z);
		}*/
	}

	std::cout << std::endl;



	//判断骨骼是否有效，骨骼记录了第一帧时物体的坐标，只有超过一帧，才会有动画
	int maxAnimKey = 0;
	for (int i = 0; i < midData.skeleton.mBoneVec.size(); i++)
	{
		if (midData.skeleton.mBoneVec[i].mFrameVec.size() > maxAnimKey)
		{
			maxAnimKey = midData.skeleton.mBoneVec[i].mFrameVec.size();
		}
	}
	if (maxAnimKey > 1)
	{
		testScene->sc.vecSkeleton.emplace_back(midData.skeleton);
	}

	//将fbx节点转化为场景节点
	std::shared_ptr<SceneNode> scNode = MiddleNode2SceneNode(nullptr, midData.rootNode, midData, testScene);
	if (maxAnimKey > 1)
	{
		testScene->SetAsSkinRoot(scNode, maxAnimKey);
	}

	//输出fbx场景树
	std::cout << std::endl;
	std::cout << "Hierarchy:" << std::endl;
	Scene::ShowHierarchy(scNode, 0);
	std::cout << std::endl;

	//对于每个fbx场景中的节点，对obj组件申请资源，设置mesh组件指向的具体mesh
	Scene::DoForNdAndAllChild(
		scNode,
		[&testScene, oldMeshNum, oldSkinMeshNum, maxAnimKey](std::shared_ptr<SceneNode> nd)
		{
			Cmpt_ObjCb* cmptObjCb = nd->Get<Cmpt_ObjCb>();
			Cmpt_Mesh* cmptMesh = nd->Get<Cmpt_Mesh>();
			Cmpt_SkinMesh* cmptSkinMesh = nd->Get<Cmpt_SkinMesh>();
			if (cmptMesh != nullptr && cmptObjCb != nullptr)
			{
				testScene->SetAsRenderItem(nd, oldMeshNum + cmptMesh->mMeshIdx, RenderLayer::NonePbr, 0);
			}
			if (cmptSkinMesh != nullptr && cmptObjCb != nullptr && maxAnimKey > 1)
			{
				testScene->SetAsSkinRenderItem(nd, oldSkinMeshNum + cmptSkinMesh->mSkinMeshIdx, RenderLayer::NonePbr, 0);

				/*WIN32DEBUG(L"------%s---------\n", RWin32::Win32_Str2Ws(nd->name).c_str());
				auto mat = testScene->sc.vecSkeleton[nd->Get<Cmpt_SkinMesh>()->mSkeletonIdx].GetSkinMat(10);
				for (int i = 0; i < mat.size(); i++)
				{
					WIN32DEBUG(L"%s %d\n",RWin32::Win32_Str2Ws(nd->name).c_str(),i);
					RDx12::MathHelper::ShowMat(mat[i]);
				}*/
			}
		}
	);

	//将fbx场景根节点置入当前场景
	Scene::AddChild(testScene->sc.root, scNode);
}

std::shared_ptr<SceneNode> RFbxMiddle2MyScene::MiddleNode2SceneNode(std::shared_ptr<SceneNode> fatherNode, RFbxNode& rNode, RFbxMidData& fbxMiddle, TestScene001_Base* testScene)
{
	auto nd = std::make_shared<SceneNode>(rNode.mName, testScene->world);
	Cmpt_Trans trans = GetTrans(rNode, fatherNode, true);
	nd->Set<Cmpt_Trans>(&trans);

	switch (rNode.mNodeType)
	{
	case FbxNodeAttribute::eSkeleton:
		break;
	case FbxNodeAttribute::eMesh:
		if (rNode.mSkin.size() > 0)
		{
			nd->Add<Cmpt_SkinMesh, Cmpt_ObjCb>();
			nd->Get<Cmpt_SkinMesh>()->mSkinMeshIdx = rNode.mSkinMeshIdx;
		}
		else
		{
			nd->Add<Cmpt_Mesh, Cmpt_ObjCb>();
			nd->Get<Cmpt_Mesh>()->mMeshIdx = rNode.mMeshIdx;
		}
		break;
	default:
		break;
	}

	for (auto& child : rNode.mChild)
	{
		Scene::AddChild(nd, MiddleNode2SceneNode(nd, child, fbxMiddle, testScene));
	}

	return nd;
}

Cmpt_Trans RFbxMiddle2MyScene::GetTrans(RFbxNode& rNode, std::shared_ptr<SceneNode> fatherNode, bool yUporzUp)
{
	Cmpt_Trans trans;

	if (fatherNode != nullptr)
	{
		if (fatherNode->name == "RootNode")
		{
			if (yUporzUp == true)//yUp
			{
				trans.mOffsetPos = DirectX::XMFLOAT3(rNode.mOffsetPosX, rNode.mOffsetPosY, -rNode.mOffsetPosZ);
			}
			else//Zup
			{
				trans.mOffsetPos = DirectX::XMFLOAT3(rNode.mOffsetPosX, rNode.mOffsetPosZ, rNode.mOffsetPosY);
			}
		}
		else
		{
			trans.mOffsetPos = DirectX::XMFLOAT3(rNode.mOffsetPosX, rNode.mOffsetPosZ, rNode.mOffsetPosY);
		}

		trans.mOffsetScale = DirectX::XMFLOAT3(rNode.mOffsetScaleX, rNode.mOffsetScaleZ, rNode.mOffsetScaleY);

		trans.mOffsetQuat = RDx12::MathHelper::FbxEuler2DxQuat(rNode.mOffsetEulerX, rNode.mOffsetEulerY, rNode.mOffsetEulerZ);
	}
	return trans;
}


