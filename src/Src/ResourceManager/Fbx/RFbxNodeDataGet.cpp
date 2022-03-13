#include "../../../Include/ResourceManage/Fbx/RFbxNodeDataGet.h"

RDx12::PureMesh<RDx12::VertexData> RFbxNodeDataGet::GetPureMesh(
	FbxNode* fbxNode,
	std::vector<DirectX::XMFLOAT3>& vertexBoneWeight,
	std::vector<DirectX::XMUINT4>& vertexBoneIdx,
	const std::vector<DirectX::XMFLOAT3>& controlPointBoneWeight,
	const std::vector<DirectX::XMUINT4>& controlPointBoneIdx
)
{
	//顶点要经过GeoTransofrmMat变换，才能得到正确位置的mesh
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (fbxNode->GetNodeAttribute())
	{
		const FbxVector4 lT = fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = fbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = fbxNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}

	RDx12::PureMesh<RDx12::VertexData> pureMesh;
	pureMesh.name = fbxNode->GetName();
	pureMesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FbxMesh* pMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	int lPolygonCount = pMesh->GetPolygonCount();
	int vertexId = 0;

	/*
	如果用3dMax生成一个立方体，更改修改器为“可编辑网格”，会得到8个控制点，12个面（三角形），若改为“可编辑多边形”，会得到8个控制点，6个面（四边形）
	*/
	//std::cout << "lPolygonCount: " << pMesh->GetPolygonCount() << std::endl;//获取网格多边形数量（一般是三边形或四边形），对应3dmax右键--属性--面数
	//std::cout << "ControlPointsCount: " << pMesh->GetControlPointsCount() << std::endl;//获取网格控制点数量（只有pos属性的点），对应3dmax右键--属性--点数
	//std::cout << "VertexColorCount: " << pMesh->GetElementVertexColorCount() << std::endl;
	//std::cout << "UVCount: " << pMesh->GetElementUVCount() << std::endl;
	//std::cout << "NormalCount: " << pMesh->GetElementNormalCount() << std::endl;

	for (int i = 0; i < lPolygonCount; i++)
	{
		int lPolygonSize = pMesh->GetPolygonSize(i);//获取第i个多边形中的顶点数量
		//std::cout << "lPolygonSize: "<< lPolygonSize << std::endl;
		for (int j = 0; j < lPolygonSize; j++)
		{

			RDx12::VertexData vertexData;
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);


			/*Pos*/
			if (lControlPointIndex < 0)
			{
				WIN32MSG(L"            Coordinates: Invalid index found!");
				continue;
			}
			else
			{
				FbxVector4* lControlPoints = pMesh->GetControlPoints();
				FbxVector4 fbxPos = lControlPoints[lControlPointIndex];
				fbxPos = matrixGeo.MultT(fbxPos);//顶点要经过GeoTransofrmMat变换，才能得到正确位置的mesh
				vertexData.pos = DirectX::XMFLOAT3(fbxPos[0], fbxPos[1], fbxPos[2]);
			}


			/*Color*/
			//std::cout << "VertexColorCount: " << pMesh->GetElementVertexColorCount() << std::endl;
			for (int l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);

				switch (leVtxc->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						FbxColor fbxColor = leVtxc->GetDirectArray().GetAt(lControlPointIndex);
						vertexData.col = DirectX::XMFLOAT4(fbxColor[0], fbxColor[1], fbxColor[2], fbxColor[3]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						FbxColor fbxColor = leVtxc->GetDirectArray().GetAt(id);
						vertexData.col = DirectX::XMFLOAT4(fbxColor[0], fbxColor[1], fbxColor[2], fbxColor[3]);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						FbxColor fbxColor = leVtxc->GetDirectArray().GetAt(vertexId);
						vertexData.col = DirectX::XMFLOAT4(fbxColor[0], fbxColor[1], fbxColor[2], fbxColor[3]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(vertexId);
						FbxColor fbxColor = leVtxc->GetDirectArray().GetAt(id);
						vertexData.col = DirectX::XMFLOAT4(fbxColor[0], fbxColor[1], fbxColor[2], fbxColor[3]);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}


			/*UV*/
			//std::cout << "UVCount: " << pMesh->GetElementUVCount() << std::endl;
			for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);

				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						FbxVector2 fbxUV = leUV->GetDirectArray().GetAt(lControlPointIndex);
						vertexData.texc = DirectX::XMFLOAT2(fbxUV[0], fbxUV[1]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						FbxVector2 fbxUV = leUV->GetDirectArray().GetAt(id);
						vertexData.texc = DirectX::XMFLOAT2(fbxUV[0], fbxUV[1]);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						FbxVector2 fbxUV = leUV->GetDirectArray().GetAt(lTextureUVIndex);
						vertexData.texc = DirectX::XMFLOAT2(fbxUV[0], fbxUV[1]);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}


			/*Normal*/
			//std::cout << "NormalCount: " << pMesh->GetElementNormalCount() << std::endl;
			for (int l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						FbxVector4 fbxNormal = leNormal->GetDirectArray().GetAt(vertexId);
						vertexData.nor = DirectX::XMFLOAT3(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
						/*    3dMax中选多个面片，选取平滑组，会影响到处多边形的顶点法向量    */
					   /* WIN32DEBUG(L"\n");
						WIN32DEBUG(L"%d %.2f %.2f %.2f\n", vertexId,vertexData.nor.x, vertexData.nor.y, vertexData.nor.z);
						WIN32DEBUG(L"\n");  */
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						FbxVector4 fbxNormal = leNormal->GetDirectArray().GetAt(id);
						vertexData.nor = DirectX::XMFLOAT3(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}


			/*Tangent，无需装填，可以通过计算获得*/
			//std::cout << "TangentCount: " << pMesh->GetElementTangentCount() << std::endl;
			//for (int l = 0; l < pMesh->GetElementTangentCount(); ++l)
			//{
			//    FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);

			//    if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			//    {
			//        switch (leTangent->GetReferenceMode())
			//        {
			//        case FbxGeometryElement::eDirect:
			//            {
			//                FbxVector4 fbxTan = leTangent->GetDirectArray().GetAt(vertexId);
			//                vertexData.tan = DirectX::XMFLOAT3(fbxTan[0], fbxTan[1], fbxTan[2]);
			//            }
			//            break;
			//        case FbxGeometryElement::eIndexToDirect:
			//            {
			//                int id = leTangent->GetIndexArray().GetAt(vertexId);
			//                FbxVector4 fbxTan = leTangent->GetDirectArray().GetAt(id);
			//                vertexData.tan = DirectX::XMFLOAT3(fbxTan[0], fbxTan[1], fbxTan[2]);
			//            }
			//            break;
			//        default:
			//            break; // other reference modes not shown here!
			//        }
			//    }

			//}


			/*Binormal，无需装填，可以通过计算获得*/
			//for (int l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			//{

			//    FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

			//    if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			//    {
			//        switch (leBinormal->GetReferenceMode())
			//        {
			//        case FbxGeometryElement::eDirect:
			//            {
			//                FbxVector4 fbxBin = leBinormal->GetDirectArray().GetAt(vertexId);
			//                vertexData.bin = DirectX::XMFLOAT3(fbxBin[0], fbxBin[1], fbxBin[2]);
			//            }
			//            break;
			//        case FbxGeometryElement::eIndexToDirect:
			//            {
			//                int id = leBinormal->GetIndexArray().GetAt(vertexId);
			//                FbxVector4 fbxBin = leBinormal->GetDirectArray().GetAt(id);
			//                vertexData.bin = DirectX::XMFLOAT3(fbxBin[0], fbxBin[1], fbxBin[2]);
			//            }
			//            break;
			//        default:
			//            break; // other reference modes not shown here!
			//        }
			//    }
			//}

			pureMesh.vecVertex.push_back(FbxVertex2DxVertex(vertexData));

			if (controlPointBoneWeight.size() > 0)
			{
				vertexBoneWeight.push_back(controlPointBoneWeight[lControlPointIndex]);
				vertexBoneIdx.push_back(controlPointBoneIdx[lControlPointIndex]);
			}

			vertexId++;

		}// for polygonSize

		/*Index*/
		int startIdx = vertexId - lPolygonSize;
		if (lPolygonSize == 4)
		{
			/*
			右手->左手坐标系，要保持法线正确，需对fbx原始点序取反
			原始                   取反
			0->1->2               0->3->2
			2->3->0               2->1->0

			  1          2
				—————
				|        |
				|        |
				|        |
				—————
			  0           3
			*/
			pureMesh.vecIndex.push_back(startIdx + 0);
			pureMesh.vecIndex.push_back(startIdx + 3);
			pureMesh.vecIndex.push_back(startIdx + 2);


			pureMesh.vecIndex.push_back(startIdx + 2);
			pureMesh.vecIndex.push_back(startIdx + 1);
			pureMesh.vecIndex.push_back(startIdx + 0);


			/* std::cout << startIdx<<std::endl;
			 std::cout << pureMesh.vecVertex[startIdx + 0].realOffset.x <<" "<< pureMesh.vecVertex[startIdx + 0].realOffset.y <<" "<< pureMesh.vecVertex[startIdx + 0].realOffset.z<<" "<< pureMesh.vecVertex[startIdx + 0].tan.x <<" "<< pureMesh.vecVertex[startIdx + 0].tan.y<<" "<< pureMesh.vecVertex[startIdx + 0].tan.z << std::endl;
			 std::cout << pureMesh.vecVertex[startIdx + 1].realOffset.x << " " << pureMesh.vecVertex[startIdx + 1].realOffset.y << " " << pureMesh.vecVertex[startIdx + 1].realOffset.z << " " << pureMesh.vecVertex[startIdx + 1].tan.x <<" "<< pureMesh.vecVertex[startIdx + 1].tan.y <<" "<< pureMesh.vecVertex[startIdx + 0].tan.z << std::endl;
			 std::cout << pureMesh.vecVertex[startIdx + 2].realOffset.x << " " << pureMesh.vecVertex[startIdx + 2].realOffset.y << " " << pureMesh.vecVertex[startIdx + 2].realOffset.z << " " << pureMesh.vecVertex[startIdx + 2].tan.x <<" "<< pureMesh.vecVertex[startIdx + 2].tan.y <<" "<< pureMesh.vecVertex[startIdx + 0].tan.z << std::endl;
			 std::cout << pureMesh.vecVertex[startIdx + 3].realOffset.x << " " << pureMesh.vecVertex[startIdx + 3].realOffset.y << " " << pureMesh.vecVertex[startIdx + 3].realOffset.z << " " << pureMesh.vecVertex[startIdx + 3].tan.x << " " << pureMesh.vecVertex[startIdx + 3].tan.y <<" "<<pureMesh.vecVertex[startIdx + 0].tan.z<< std::endl;*/

		}
		else if (lPolygonSize == 3)
		{
			pureMesh.vecIndex.push_back(startIdx + 2);
			pureMesh.vecIndex.push_back(startIdx + 1);
			pureMesh.vecIndex.push_back(startIdx + 0);
		}

	}// for polygonCount

	return pureMesh;
}


/*
Fbx(3dMax，右手坐标系)数据到Dx12左手坐标系的转换

  3dMax                       MyDx
  Forward = Y                 Forward = Z
  Right = X                   Right = X
  Up = Z                      Up = Y

										右手坐标系                  左手坐标系
  Raw(根据左右手定律绕Forward旋转):      Z->X为正数                  X->Y为正数
  Pitch(根据左右手定律绕Right旋转):      Y->Z为正数                  Y->Z为正数
  Yaw(根据注意右手定律绕Up旋转):         X->Y为正数                  Z->X为正数

  方向朝Down的法线:
									  顺时针顺序的点
									  A X=-1,Y=-1                   X=-1,Z=-1
									  B X=-1,Y=1                    X=-1,Z=1
									  C X=1,Y=1                     X=1,Z=1
									  D X=1,Y=-1                    X=1,Z=-1
									根据右手定律A->B->C->D      根据左手定律 D->C->B->A
									 Normal:X=0,Y=0,Z=-1         Normal:X=0,Y=-1,Z=0

  对比上述可得
	   3dMax        MyDx
  Pos:(X,Y,Z)---> (X,Z,Y);

									3dMax                                MyDx
										   (交换ZY)            (取反)
  Raw(根据右左手绕Forward旋转):   Z->X为正 --------> Y->X为正 --------> Y->X为负

										   (交换ZY)            (取反)
  Pitch(根据右左手绕Right旋转):   Y->Z为正 --------> Z->Y为正 --------> Z->Y为负

										   (交换ZY)            (取反)
  Yaw(根据右左手绕Up旋转):        X->Y为正 --------> X->Z为正 --------> X->Z为负

											   (交换ZY)                      (点序取反)
  Normal:                         X=0,Y=0,Z=-1 -------> X=0,Y=-1,Z=0 -------> DCBA

  Uv:                            (u,v)----------->(u,1-v);

  Tan:

	Z                         Y
	/\    Y                   /\    Z
	|   /|                    |   /|
	|  /                      |  /
	| /                       | /
	|/___________>            |/___________>
				   X                         X


	3dmax Uv                Dx Uv
	 (0,1)       (1,1)       (0,0)       (1,0)
	   —————              —————
	   |        |              |        |
	   |        |              |        |
	   |        |              |        |
	   —————              —————
	 (0,0)       (1,0)       (0,1)       (1,1)
*/
RDx12::VertexData RFbxNodeDataGet::FbxVertex2DxVertex(RDx12::VertexData fbxVt)
{
	RDx12::VertexData myDxVt;
	myDxVt.pos = DirectX::XMFLOAT3(fbxVt.pos.x, fbxVt.pos.z, fbxVt.pos.y);
	myDxVt.col = fbxVt.col;
	myDxVt.nor = DirectX::XMFLOAT3(fbxVt.nor.x, fbxVt.nor.z, fbxVt.nor.y);
	myDxVt.tan = DirectX::XMFLOAT3(fbxVt.tan.x, fbxVt.tan.z, fbxVt.tan.y);
	myDxVt.texc = DirectX::XMFLOAT2(fbxVt.texc.x, 1 - fbxVt.texc.y);
	return myDxVt;
}

void RFbxNodeDataGet::GetTransMidData(RFbxNode* rNode, FbxObject* pObject)
{
	// 遍历节点属性
	FbxProperty lProperty = pObject->GetFirstProperty();

	while (lProperty.IsValid())
	{
		FbxString lString = lProperty.GetName();
		FbxDataType lPropertyDataType = lProperty.GetPropertyDataType();

		if (lPropertyDataType.GetType() == eFbxDouble3 || lPropertyDataType.GetType() == eFbxDouble4)
		{
			FbxDouble3 lDefault = lProperty.Get<FbxDouble3>();

			//Yup，Zup只影响Trans
			if (lString == "Lcl Translation")
			{
				rNode->mOffsetPosX = lDefault[0];
				rNode->mOffsetPosY = lDefault[1];
				rNode->mOffsetPosZ = lDefault[2];
			}
			else if (lString == "Lcl Rotation")
			{
				rNode->mOffsetEulerX = lDefault[0];
				rNode->mOffsetEulerY = lDefault[1];
				rNode->mOffsetEulerZ = lDefault[2];
			}
			else if (lString == "Lcl Scaling")
			{
				rNode->mOffsetScaleX = lDefault[0];
				rNode->mOffsetScaleY = lDefault[1];
				rNode->mOffsetScaleZ = lDefault[2];
			}
		}
		lProperty = pObject->GetNextProperty(lProperty);
	}
}


std::vector<std::vector<RDx12::AnimCurve>> RFbxNodeDataGet::GetAnimMidData(FbxNode* lNode, FbxScene* lScene)
{
	std::vector<std::vector<RDx12::AnimCurve>> res;

	int animStackCount = lScene->GetSrcObjectCount<FbxAnimStack>();
	res.resize(animStackCount);

	for (int i = 0; i < animStackCount; i++)
	{
		FbxAnimStack* lAnimStack = lScene->GetSrcObject<FbxAnimStack>(i);

		int animLayerCount = lAnimStack->GetMemberCount<FbxAnimLayer>();
		res[i].resize(animLayerCount);

		for (int j = 0; j < animLayerCount; j++)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			res[i][j] = RFbxAnimDataGet::GetAnimCurve(lNode, lAnimLayer);
		}
	}

	return res;
}

std::vector<std::vector<RFbxLink>> RFbxNodeDataGet::GetSkinMidData(FbxNode* fbxNode)
{
	std::vector<std::vector<RFbxLink>> res;

	FbxMesh* pMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	res.resize(lSkinCount);

	for (int i = 0; i < lSkinCount; i++)
	{
		FbxSkin* lSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
		int lClusterCount = lSkin->GetClusterCount();
		res[i].resize(lClusterCount);

		for (int j = 0; j < lClusterCount; j++)
		{
			FbxCluster* lCluster = lSkin->GetCluster(j);
			res[i][j] = RFbxAnimDataGet::GetSkinLink(lCluster);
		}
	}

	return res;
}