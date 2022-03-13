#include "../../../Include/ResourceManage/Fbx/RFbxAnimDataGet.h"

float RFbxCurve::Interpolate(float t, float defaultRes)
{
	float res = defaultRes;
	if (mKvVec.size() != 0)
	{
		if (t <= mKvVec.front().mTime)
		{
			res = mKvVec.front().mVal;
		}
		else if (t >= mKvVec.back().mTime)
		{
			res = mKvVec.back().mVal;
		}
		else
		{
			for (int i = 0; i < mKvVec.size() - 1; ++i)
			{
				if (t >= mKvVec[i].mTime && t <= mKvVec[i + 1].mTime)
				{
					float lerpPercent = (t - mKvVec[i].mTime) / (mKvVec[i + 1].mTime - mKvVec[i].mTime);
					res = mKvVec[i].mVal + lerpPercent * (mKvVec[i + 1].mVal - mKvVec[i].mVal);
				}
			}
		}
	}
	return res;
}

RDx12::AnimCurve RFbxAnimDataGet::GetAnimCurve(FbxNode* pNode, FbxAnimLayer* pAnimLayer)
{
	FbxAnimCurve* lAnimCurve = NULL;

	int maxFrameCount = 0;
	FbxTime::EMode eMode = FbxTime::EMode::eDefaultMode;
	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}

	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}

	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}
	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetDurationAndInterval(lAnimCurve, maxFrameCount, eMode);
	}

	return GetAnimCurve(pNode, maxFrameCount + 1, eMode, true);//至少有一帧用来记录初始姿态
}

RFbxCurve RFbxAnimDataGet::GetCurve(FbxAnimCurve* lAnimCurve)
{
	RFbxCurve rCurve;

	int lKeyCount = lAnimCurve->KeyGetCount();

	for (int i = 0; i < lKeyCount; i++)
	{
		float lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(i));
		FbxTime lKeyTime = lAnimCurve->KeyGetTime(i);

		RFbxKv kv;
		char lTimeString[256];
		kv.mTime = std::atoi(lKeyTime.GetTimeString(lTimeString, FbxUShort(256)));
		kv.mVal = lKeyValue;
		rCurve.mKvVec.emplace_back(kv);
	}

	return rCurve;
}

void RFbxAnimDataGet::GetDurationAndInterval(FbxAnimCurve* lAnimCurve, int& maxFrameCount, FbxTime::EMode& eMode)
{
	FbxTimeSpan span;
	lAnimCurve->GetTimeInterval(span);
	eMode = span.GetDuration().GetGlobalTimeMode();
	int curveFrameCount = span.GetDuration().GetFrameCount(eMode);
	maxFrameCount = curveFrameCount > maxFrameCount ? curveFrameCount : maxFrameCount;
}

RDx12::AnimCurve RFbxAnimDataGet::GetAnimCurve(FbxNode* pNode, int maxFrameCount, FbxTime::EMode eMode, bool yUporzUp)
{
	RDx12::AnimCurve rCurve;
	rCurve.mFrameVec.resize(maxFrameCount);
	std::cout << eMode << std::endl;
	for (int i = 0; i < maxFrameCount; i++)
	{
		FbxTime t;
		t.SetFrame(i, eMode);

		/*
		//获得的Transform矩阵是包括preRotation的，受到“Y轴朝上、Z轴朝上导出”的影响
		auto fbxMat = pNode->GetAnimationEvaluator()->GetNodeLocalTransform(pNode, t);
		if (yUporzUp)
		{
			for (int i = 0; i < 4; i++)
			{
				double temp = fbxMat.mData[i].mData[1];
				fbxMat.mData[i].mData[1] = -fbxMat.mData[i].mData[2];
				fbxMat.mData[i].mData[2] = temp;
			}
		}
		std::cout << std::endl;
		std::cout << fbxMat.GetT().Buffer()[0] << "," << fbxMat.GetT().Buffer()[1] << "," << fbxMat.GetT().Buffer()[2] << std::endl;
		std::cout << fbxMat.GetR().Buffer()[0] << "," << fbxMat.GetR().Buffer()[1] << "," << fbxMat.GetR().Buffer()[2] << std::endl;
		std::cout << fbxMat.GetS().Buffer()[0] << "," << fbxMat.GetS().Buffer()[1] << "," << fbxMat.GetS().Buffer()[2] << std::endl;
		std::cout << std::endl;
		*/



		//下面三个函数不受preRotation到影响
		auto pos = pNode->GetAnimationEvaluator()->GetNodeLocalTranslation(pNode, t);
		auto euler = pNode->GetAnimationEvaluator()->GetNodeLocalRotation(pNode, t);
		auto scale = pNode->GetAnimationEvaluator()->GetNodeLocalScaling(pNode, t);
		
		/*
		if (i == 10)
		{
			std::cout << "Local Pos    " <<pNode->GetName()<< std::endl;
			std::cout << pos.Buffer()[0] << "," << pos.Buffer()[1] << "," << pos.Buffer()[2] << std::endl;
			std::cout << euler.Buffer()[0] << "," << euler.Buffer()[1] << "," << euler.Buffer()[2] << std::endl;
			std::cout << scale.Buffer()[0] << "," << scale.Buffer()[1] << "," << scale.Buffer()[2] << std::endl; 
			std::cout << std::endl;
		}
		*/

		rCurve.mFrameVec[i].TimePos = i;
		rCurve.mFrameVec[i].mTrans = DirectX::XMFLOAT3(0, 0, 0);
		rCurve.mFrameVec[i].mScale = DirectX::XMFLOAT3(1, 1, 1);
		rCurve.mFrameVec[i].mQuat = DirectX::XMFLOAT4(0, 0, 0, 1);
		if (pNode->GetParent() != nullptr)
		{
			//std::cout << pNode->GetParent()->GetName() << std::endl;
			if (std::string(pNode->GetParent()->GetName()) == "RootNode")
			{
				if (yUporzUp == true)//yUp
				{
					rCurve.mFrameVec[i].mTrans = DirectX::XMFLOAT3(pos.Buffer()[0], pos.Buffer()[1], -pos.Buffer()[2]);
				}
				else//Zup
				{
					rCurve.mFrameVec[i].mTrans = DirectX::XMFLOAT3(pos.Buffer()[0], pos.Buffer()[2], pos.Buffer()[1]);
				}
			}
			else
			{
				rCurve.mFrameVec[i].mTrans = DirectX::XMFLOAT3(pos.Buffer()[0], pos.Buffer()[2], pos.Buffer()[1]);
			}

			rCurve.mFrameVec[i].mScale = DirectX::XMFLOAT3(scale.Buffer()[0], scale.Buffer()[2], scale.Buffer()[1]);

			rCurve.mFrameVec[i].mQuat = RDx12::MathHelper::FbxEuler2DxQuat(euler.Buffer()[0], euler.Buffer()[1], euler.Buffer()[2]);
		}

		/*
		std::cout << rCurve.mKvVec[i].mTrans.x << "," << rCurve.mKvVec[i].mTrans.y << "," << rCurve.mKvVec[i].mTrans.z << std::endl;
		std::cout << std::endl;
		*/
	}

	return rCurve;
}

RFbxLink RFbxAnimDataGet::GetSkinLink(FbxCluster* lCluster)
{
	RFbxLink rLink;
	FbxNode* link = lCluster->GetLink();
	if (link != NULL)
	{
		rLink.mLinkNodeUniqueId = link->GetUniqueID();
	}

	int lIndexCount = lCluster->GetControlPointIndicesCount();
	int* lIndices = lCluster->GetControlPointIndices();
	double* lWeights = lCluster->GetControlPointWeights();

	//获取此Link关系的节点和权重
	for (int k = 0; k < lIndexCount; k++)
	{
		RFbxIdxAndWeight idxAndWeight;
		idxAndWeight.mIdx = lIndices[k];
		idxAndWeight.mWeight = (float)lWeights[k];
		rLink.mIdxAndWeight.emplace_back(idxAndWeight);
	}

	bool YuporZup = true;
	FbxAMatrix lMatrix;

	//获取绑定时此Mesh的worldTransform
	lMatrix = lCluster->GetTransformMatrix(lMatrix);

	/*
	std::cout << lMatrix.mData[0].mData[0] <<" "<< lMatrix.mData[0].mData[1] <<" "<< lMatrix.mData[0].mData[2] <<" "<< lMatrix.mData[0].mData[3]<<std::endl;
	std::cout << lMatrix.mData[1].mData[0] << " " << lMatrix.mData[1].mData[1] << " " << lMatrix.mData[1].mData[2] << " " << lMatrix.mData[1].mData[3] << std::endl;
	std::cout << lMatrix.mData[2].mData[0] << " " << lMatrix.mData[2].mData[1] << " " << lMatrix.mData[2].mData[2] << " " << lMatrix.mData[2].mData[3] << std::endl;
	std::cout << lMatrix.mData[3].mData[0] << " " << lMatrix.mData[3].mData[1] << " " << lMatrix.mData[3].mData[2] << " " << lMatrix.mData[3].mData[3] << std::endl;
	std::cout << lMatrix.GetR().Buffer()[0] << " " << lMatrix.GetR().Buffer()[1] << " " << lMatrix.GetR().Buffer()[2] << std::endl;
	std::cout << std::endl;
	*/

	if (YuporZup)
	{
		for (int i = 0; i < 4; i++)
		{
			double temp = lMatrix.mData[i].mData[1];
			lMatrix.mData[i].mData[1] = -lMatrix.mData[i].mData[2];
			lMatrix.mData[i].mData[2] = temp;
		}
	}
	
	/*
	std::cout << lMatrix.mData[0].mData[0] << " " << lMatrix.mData[0].mData[1] << " " << lMatrix.mData[0].mData[2] << " " << lMatrix.mData[0].mData[3] << std::endl;
	std::cout << lMatrix.mData[1].mData[0] << " " << lMatrix.mData[1].mData[1] << " " << lMatrix.mData[1].mData[2] << " " << lMatrix.mData[1].mData[3] << std::endl;
	std::cout << lMatrix.mData[2].mData[0] << " " << lMatrix.mData[2].mData[1] << " " << lMatrix.mData[2].mData[2] << " " << lMatrix.mData[2].mData[3] << std::endl;
	std::cout << lMatrix.mData[3].mData[0] << " " << lMatrix.mData[3].mData[1] << " " << lMatrix.mData[3].mData[2] << " " << lMatrix.mData[3].mData[3] << std::endl;
	std::cout << lMatrix.GetR().Buffer()[0] << " " << lMatrix.GetR().Buffer()[1] << " " << lMatrix.GetR().Buffer()[2] << std::endl;
	std::cout << std::endl;
	*/

	rLink.mMeshBindWorldMat = RDx12::MathHelper::FbxTrans2DxMat(
		!YuporZup,
		lMatrix.GetT().Buffer()[0], lMatrix.GetT().Buffer()[1], lMatrix.GetT().Buffer()[2],
		lMatrix.GetS().Buffer()[0], lMatrix.GetS().Buffer()[1], lMatrix.GetS().Buffer()[2],
		lMatrix.GetR().Buffer()[0], lMatrix.GetR().Buffer()[1], lMatrix.GetR().Buffer()[2]
	);

	//获取绑定时此LinkNode的worldTransform
	lMatrix = lCluster->GetTransformLinkMatrix(lMatrix);
	if (YuporZup)
	{
		for (int i = 0; i < 4; i++)
		{
			double temp = lMatrix.mData[i].mData[1];
			lMatrix.mData[i].mData[1] = -lMatrix.mData[i].mData[2];
			lMatrix.mData[i].mData[2] = temp;
		}
	}

	rLink.mLinkNodeBindWorldMat = RDx12::MathHelper::FbxTrans2DxMat(
		!YuporZup,
		lMatrix.GetT().Buffer()[0], lMatrix.GetT().Buffer()[1], lMatrix.GetT().Buffer()[2],
		lMatrix.GetS().Buffer()[0], lMatrix.GetS().Buffer()[1], lMatrix.GetS().Buffer()[2],
		lMatrix.GetR().Buffer()[0], lMatrix.GetR().Buffer()[1], lMatrix.GetR().Buffer()[2]
	);
	rLink.mLinkNodeBindWorldQuat = RDx12::MathHelper::FbxEuler2DxQuat(lMatrix.GetR().Buffer()[0], lMatrix.GetR().Buffer()[1], lMatrix.GetR().Buffer()[2]);
	/*
	std::cout << "Bind Pose    "<<link->GetName() << std::endl;
	std::cout << lMatrix.GetT().Buffer()[0] << "," << lMatrix.GetT().Buffer()[1] << "," << lMatrix.GetT().Buffer()[2] << std::endl;
	std::cout << lMatrix.GetS().Buffer()[0] << "," << lMatrix.GetS().Buffer()[1] << "," << lMatrix.GetS().Buffer()[2] << std::endl;
	std::cout << lMatrix.GetR().Buffer()[0] << "," << lMatrix.GetR().Buffer()[1] << "," << lMatrix.GetR().Buffer()[2] << std::endl;
	auto euler = RDx12::MathHelper::Quat2EluerDeg(RDx12::MathHelper::FbxEuler2DxQuat(lMatrix.GetR().Buffer()[0], lMatrix.GetR().Buffer()[1], lMatrix.GetR().Buffer()[2]));
	std::cout << std::endl;
	*/

	//RDx12::MathHelper::ShowMat(rLink.mLinkNodeBindWorldMat);

	return rLink;
}