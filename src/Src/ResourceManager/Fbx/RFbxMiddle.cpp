#include "../../../Include/ResourceManage/Fbx/RFbxMiddle.h"

void RFbxMidData::GetSkeleton(FbxScene* lScene)
{
	std::queue<std::pair<FbxNode*, int>> nodeQue;
	nodeQue.push(std::make_pair(lScene->GetRootNode(), -1));
	std::cout << std::endl;
	std::cout << "Skeleton: " << std::endl;
	while (nodeQue.size() > 0)
	{
		FbxNode* fbxNode = nodeQue.front().first;
		int fatherIdx = nodeQue.front().second;
		nodeQue.pop();
		auto animData = RFbxNodeDataGet::GetAnimMidData(fbxNode, lScene);
		mLinkNodeId.push_back(fbxNode->GetUniqueID());
		skeleton.mBoneVec.push_back(animData[0][0]);
		skeleton.mParentVec.push_back(fatherIdx);
		skeleton.mNameVec.push_back(fbxNode->GetName());
		fatherIdx = mLinkNodeId.size() - 1;
		for (int i = 0; i < fbxNode->GetChildCount(); i++)
		{
			nodeQue.push(std::make_pair(fbxNode->GetChild(i), fatherIdx));
		}
	
		std::cout << skeleton.mParentVec.size() - 1 << " " << skeleton.mParentVec.back() << ":" << fbxNode->GetName() << std::endl;
	}
	skeleton.UpdateChildVec();
	for (int i = 0; i < skeleton.mBoneVec.size(); i++)
	{
		skeleton.mBoneBindWorldMatVec.push_back(RDx12::MathHelper::Identity4x4());
		skeleton.mBoneBindWorldQuat.push_back({0,0,0,1});
	}
	std::cout << std::endl;
}

void RFbxMidData::GetMesh(FbxScene* lScene)
{
	std::queue<FbxNode*> nodeQue;
	nodeQue.push(lScene->GetRootNode());
	std::cout << std::endl;
	std::cout << "Mesh: " << std::endl;
	while (nodeQue.size() > 0)
	{
		FbxNode* fbxNode = nodeQue.front();
		nodeQue.pop();


		if (fbxNode->GetNodeAttribute() != NULL)
		{
			FbxNodeAttribute::EType lAttributeType = (fbxNode->GetNodeAttribute()->GetAttributeType());

			switch (lAttributeType)
			{
			default:
				break;
			case FbxNodeAttribute::eMesh:
			{
				FbxMesh* pMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
				uint64_t meshId = pMesh->GetUniqueID();

				auto skinData = RFbxNodeDataGet::GetSkinMidData(fbxNode);
				std::vector<DirectX::XMFLOAT3> controlPointBoneWeight;
				std::vector<DirectX::XMUINT4> controlPointBoneIdx;

				if (skinData.size() > 0)
				{
					int controlPointNum = pMesh->GetControlPointsCount();
					controlPointBoneWeight = std::vector<DirectX::XMFLOAT3>(controlPointNum, { -1 ,-1, -1 });
					controlPointBoneIdx = std::vector<DirectX::XMUINT4>(controlPointNum, { (UINT)-1 ,(UINT)-1, (UINT)-1, (UINT)-1 });

					//遍历每一块骨头
					for (int i = 0; i < skinData[0].size(); i++)
					{
						//检查LinkNode属于Skeleton数组的第几个元素
						int nodeId = skinData[0][i].mLinkNodeUniqueId;
						int boneIdx = 0;
						for (int j = 0; j < mLinkNodeId.size(); j++)
						{
							if (nodeId == mLinkNodeId[j])
							{
								skeleton.mBoneBindWorldMatVec[j] = skinData[0][i].mLinkNodeBindWorldMat;
								skeleton.mBoneBindWorldQuat[j] = skinData[0][i].mLinkNodeBindWorldQuat;
								/*WIN32DEBUG(L"LinkNode: %d", j);
								RDx12::MathHelper::ShowMat(skeleton.mBoneBindWorldMatVec[j]);*/
								boneIdx = j;
								break;
							}
						}
						//std::cout << "NodeBind: " << fbxNode->GetName() << "--> Bone " << boneIdx << " " << skinData[0][i].mIdxAndWeight.size() << " " << controlPointBoneIdx.size() << std::endl;
						//填充蒙皮顶点数据
						for (auto& idxW : skinData[0][i].mIdxAndWeight)
						{
							int vertexIdx = idxW.mIdx;
							float boneWeight = idxW.mWeight;

							if (controlPointBoneIdx[vertexIdx].x == -1)
							{
								controlPointBoneWeight[vertexIdx].x = boneWeight;
								controlPointBoneIdx[vertexIdx].x = boneIdx;
							}
							else if (controlPointBoneIdx[vertexIdx].y == -1)
							{
								controlPointBoneWeight[vertexIdx].y = boneWeight;
								controlPointBoneIdx[vertexIdx].y = boneIdx;
							}
							else if (controlPointBoneIdx[vertexIdx].z == -1)
							{
								controlPointBoneWeight[vertexIdx].z = boneWeight;
								controlPointBoneIdx[vertexIdx].z = boneIdx;
							}
							else if (controlPointBoneIdx[vertexIdx].w == -1)
							{
								controlPointBoneIdx[vertexIdx].w = boneIdx;
							}
							else
							{
								/*	std::cout << "More Than 4 Bone in a Vertex" << std::endl;
									float weight[4];
									weight[0] = controlPointBoneWeight[vertexIdx].x;
									weight[1] = controlPointBoneWeight[vertexIdx].y;
									weight[2] = controlPointBoneWeight[vertexIdx].z;
									weight[3] = 1 - weight[0] - weight[1] - weight[2];
									int boneIdxArray[4];
									boneIdxArray[0] = controlPointBoneIdx[vertexIdx].x;
									boneIdxArray[1] = controlPointBoneIdx[vertexIdx].y;
									boneIdxArray[2] = controlPointBoneIdx[vertexIdx].z;
									boneIdxArray[3] = controlPointBoneIdx[vertexIdx].w;

									int min = 999;
									int idx = 0;
									for (int j = 0; j < 4; j++)
									{
										if (weight[j] < min)
										{
											min = weight[j];
											idx = j;
										}
									}
									weight[idx] = boneWeight;
									boneIdxArray[idx] = boneIdx;
									controlPointBoneWeight[vertexIdx].x = weight[0];
									controlPointBoneWeight[vertexIdx].y = weight[1];
									controlPointBoneWeight[vertexIdx].z = weight[2];
									controlPointBoneWeight[vertexIdx].w = weight[3];*/


							}
						}
					}
				}

				std::vector<DirectX::XMFLOAT3> vertexBoneWeight;
				std::vector<DirectX::XMUINT4> vertexBoneIdx;
				RDx12::PureMesh<RDx12::VertexData> pureMesh = RFbxNodeDataGet::GetPureMesh(fbxNode, vertexBoneWeight, vertexBoneIdx, controlPointBoneWeight, controlPointBoneIdx);

				//PureMesh去重
				int hasIdx = -1;
				for (int i = 0; i < mPureMeshId.size(); i++)
				{
					if (meshId == mPureMeshId[i])
					{
						hasIdx = i;
						break;
					}
				}

				if (hasIdx == -1)
				{
					mPureMeshVec.emplace_back(pureMesh);
					mPureMeshId.push_back(meshId);
					std::cout << pureMesh.name << " " << pureMesh.vecVertex.size() << " " << pureMesh.vecIndex.size() << std::endl;
				}

				//由于每个节点蒙皮可能不一样，所以SkinMesh基本不会重复，不用去重，用节点当Id
				if (vertexBoneWeight.size() > 0)
				{
					RDx12::PureMesh<RDx12::SkinVertexData> skinMesh;
					skinMesh.name = pureMesh.name + "_skin";
					skinMesh.topology = pureMesh.topology;
					skinMesh.vecIndex = pureMesh.vecIndex;
					for (int i = 0; i < pureMesh.vecVertex.size(); i++)
					{
						RDx12::SkinVertexData vtSkin;
						vtSkin.pos = pureMesh.vecVertex[i].pos;
						vtSkin.col = pureMesh.vecVertex[i].col;
						vtSkin.nor = pureMesh.vecVertex[i].nor;
						vtSkin.texc = pureMesh.vecVertex[i].texc;
						vtSkin.tan = pureMesh.vecVertex[i].tan;
						vtSkin.boneWeight = vertexBoneWeight[i];
						vtSkin.boneIdx = vertexBoneIdx[i];
						skinMesh.vecVertex.push_back(vtSkin);
					}
					mSkinMeshVec.push_back(skinMesh);
					mSkinMeshId.push_back(fbxNode->GetUniqueID());
					mSkinMeshBindWorldMat.push_back(skinData[0][0].mMeshBindWorldMat);
					RDx12::MathHelper::ShowMat(skinData[0][0].mMeshBindWorldMat);
					std::cout << skinMesh.name << " " << skinMesh.vecVertex.size() << " " << skinMesh.vecIndex.size() << std::endl;
				}
			}
			break;
			}
		}

		for (int i = 0; i < fbxNode->GetChildCount(); i++)
		{
			nodeQue.push(fbxNode->GetChild(i));
		}
	}
	std::cout << std::endl;
}

RFbxMidData RFbxMiddle::GetMidData(std::string path)
{
	RFbxMidData rfbxMidData;

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "|----------FbxDo---------|" << std::endl;

	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	InitializeSdkObjects(lSdkManager, lScene);

	FbxString lFilePath(path.c_str());

	if (lFilePath.IsEmpty())
	{
		lResult = false;
	}
	else
	{
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}

	if (lResult == true)
	{
		//获取SceneNode和PureMesh和Node的Anim
		rfbxMidData.GetSkeleton(lScene);
		rfbxMidData.GetMesh(lScene);
		rfbxMidData.rootNode = GetMidNodeRcs(lScene->GetRootNode(), &rfbxMidData, lScene, 0);
		//RDx12::MathHelper::ShowMat(rfbxMidData.skeleton.mBoneVec[2].LerpMat(15));
	}

	DestroySdkObjects(lSdkManager, lResult);

	std::cout << std::endl;
	std::cout << std::endl;

	return rfbxMidData;
}


RFbxNode RFbxMiddle::GetMidNodeRcs(FbxNode* fbxNode, RFbxMidData* fbxMidData, FbxScene* lScene, int depth)
{
	RFbxNode rNode;
	rNode.mName = fbxNode->GetName();
	rNode.mUniqueId = fbxNode->GetUniqueID();

	RFbxNodeDataGet::GetTransMidData(&rNode, fbxNode);
	rNode.mAnim = RFbxNodeDataGet::GetAnimMidData(fbxNode, lScene);

	if (fbxNode->GetNodeAttribute() != NULL)
	{
		FbxNodeAttribute::EType lAttributeType = (fbxNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		default:
			break;

		case FbxNodeAttribute::eSkeleton:
		{
			rNode.mNodeType = FbxNodeAttribute::eSkeleton;
		}
		break;

		case FbxNodeAttribute::eMesh:
		{
			rNode.mNodeType = FbxNodeAttribute::eMesh;
			rNode.mSkin = RFbxNodeDataGet::GetSkinMidData(fbxNode);

			FbxMesh* pMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
			uint64_t meshId = pMesh->GetUniqueID();

			if (rNode.mSkin.size() > 0)
			{
				for (int i = 0; i < fbxMidData->mSkinMeshId.size(); i++)
				{
					if (fbxNode->GetUniqueID() == fbxMidData->mSkinMeshId[i])
					{
						rNode.mSkinMeshIdx = i;
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < fbxMidData->mPureMeshId.size(); i++)
				{
					if (meshId == fbxMidData->mPureMeshId[i])
					{
						rNode.mMeshIdx = i;
						break;
					}
				}
			}
		}
		break;
		}
	}

	std::string depthPre;
	for (int i = 0; i < depth; i++)
	{
		depthPre += "    ";
	}
	std::cout << depthPre << "Node: " << rNode.mName << std::endl;
	std::cout << depthPre << "OffsetPos: " << rNode.mOffsetPosX << " " << rNode.mOffsetPosY << " " << rNode.mOffsetPosZ << std::endl;
	std::cout << depthPre << "OffsetScale: " << rNode.mOffsetScaleX << " " << rNode.mOffsetScaleY << " " << rNode.mOffsetScaleZ << std::endl;
	std::cout << depthPre << "OffsetEuler: " << rNode.mOffsetEulerX << " " << rNode.mOffsetEulerY << " " << rNode.mOffsetEulerZ << std::endl;
	
	/*
	if (rNode.mSkin.size()>0 && rNode.mSkin[0][0].mIdxAndWeight.size() != 0)
	{
		std::cout << depthPre <<"IdxAndWeight Num: "<< rNode.mSkin[0][0].mIdxAndWeight.size() << std::endl;
		std::cout << depthPre;
		for (auto& data : rNode.mSkin[0][0].mIdxAndWeight)
		{
			std::cout << data.mIdx << ":" << data.mWeight << " ";
		}
		std::cout << std::endl;
	}
	*/

	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		rNode.mChild.emplace_back(GetMidNodeRcs(fbxNode->GetChild(i), fbxMidData, lScene, depth + 1));
	}
	return rNode;
}