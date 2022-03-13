#include "../../Include/Scene/Scene.h"
void Scene::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteSerializeVec(outFile, vecPureMesh);
	RWin32::Bin_WriteSerializeVec(outFile, vecSkinMesh);
	RWin32::Bin_WriteVec(outFile, vecSkinMeshBindWorldMat);
	RWin32::Bin_WriteSerializeVec(outFile, vecSkeleton);
}

void Scene::RSerialize_Load(std::ifstream& inFile)
{
	vecPureMesh = RWin32::Bin_LoadSerializeVec<RDx12::PureMesh<RDx12::VertexData>>(inFile);
	vecSkinMesh = RWin32::Bin_LoadSerializeVec<RDx12::PureMesh<RDx12::SkinVertexData>>(inFile);
	vecSkinMeshBindWorldMat = RWin32::Bin_LoadVec<DirectX::XMFLOAT4X4>(inFile);
	vecSkeleton = RWin32::Bin_LoadSerializeVec<RDx12::Skeleton>(inFile);
}

bool Scene::AddChild(std::shared_ptr<SceneNode> father, std::shared_ptr<SceneNode> child)
{
	//���߲���Ϊ�գ��Ҹ��Ӳ���ͬһ��node
	if (father == nullptr || child == nullptr || father == child)
		return false;

	/*
	��father��child����һ����֧��ʱ��    Childɾ�����ף�����father��child�б�
     									        			  	         
     	     A                                    			     A                
           /  \                                   		          \               
     	  /    \						          			       \				
     	B(Child)C						        	B(Child)     	C				
       / \     / \ 						           / \    		   / \ 				
      /   \   /   \						          /   \   		  /   \				
     D     E F     G��father��			         D     E  		 F     G��father��

	
	��father��child��һ����֧����father��child�����ȼ�         
			 A(father)											
		   /  \													
		  /    \												
		 B      C												
	   / \     / \												
	  /   \   /   \												
	 D     E F     G��child��									
	
	 Childɾ�����ף�����father��child�б�      
			 A(father)											
		   /  \													
		  /    \												
		 B      C												
	   / \     / 												
	  /   \   /   												
	 D     E F     G��child��															


	 ��father��child��һ����֧����child��father�����ȼ�,��3dmax�лᱨ��Cannot link��create dependency loop��
			 A(child)
		   /  \
		  /    \
		 B      C
	   / \     / \
	  /   \   /   \
	 D     E F     G��father��

	*/

	int sameRoute = IsSameRoute(father, child,child);

	if (sameRoute == 2)
	{
		std::cout << "Cannot link��create dependency loop" << std::endl;
		return false;
	}

	if (child->parent!=nullptr)
	{
		child->parent->child.remove(child);
	}
	child->parent = father;
	father->child.push_back(child);
	
	child->UpdateWorldTranByOffset();

	return true;
}

bool Scene::AddChildKeepWorldMat(std::shared_ptr<SceneNode> father, std::shared_ptr<SceneNode> child)
{
	//���߲���Ϊ�գ��Ҹ��Ӳ���ͬһ��node
	if (father == nullptr || child == nullptr || father == child)
		return false;

	int sameRoute = IsSameRoute(father, child, child);

	if (sameRoute == 2)
	{
		std::cout << "Cannot link��create dependency loop" << std::endl;
		return false;
	}

	if (child->parent != nullptr)
	{
		child->parent->child.remove(child);
	}
	child->parent = father;
	father->child.push_back(child);

	Cmpt_Trans* trans = child->Get<Cmpt_Trans>();
	Cmpt_Trans* fatherTrans = father->Get<Cmpt_Trans>();

	//���ĸ��ӹ�ϵ�����ӽڵ��offset��������̬�������ģ�����ڸ��ĸ���󱣳�������̬�д���������
	DirectX::XMFLOAT4X4 newOffSetMat = RDx12::MathHelper::MulMat(trans->mWorldMat, RDx12::MathHelper::InverMat(fatherTrans->mWorldMat));
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 quat;
	RDx12::MathHelper::MatDeCompose(newOffSetMat, pos, trans->mOffsetScale, quat);

	trans->mOffsetPos = RDx12::MathHelper::F3MulMat(trans->mPos, RDx12::MathHelper::InverMat(fatherTrans->mWorldMat));
	trans->mOffsetQuat = RDx12::MathHelper::QuatMulQuat(trans->mQuat, RDx12::MathHelper::QuatInver(fatherTrans->mQuat));
	//RDx12::MathHelper::ShowFloat3(RDx12::MathHelper::F3MulMat(trans->mOffsetPos, fatherTrans->mWorldMat));
	//RDx12::MathHelper::ShowFloat4(trans->mQuat);
	//RDx12::MathHelper::ShowFloat4(RDx12::MathHelper::QuatMulQuat(trans->mOffsetQuat, fatherTrans->mQuat));
	//RDx12::MathHelper::ShowFloat3(RDx12::MathHelper::Quat2EluerDeg(trans->mOffsetQuat));
	
	DirectX::XMFLOAT4X4 SRTMat;
	DirectX::XMStoreFloat4x4(
		&SRTMat,
		RDx12::MathHelper::GetTransMatByF(
			{ 0,0,0 },
			{ 0,0,0,1 },
			trans->mOffsetScale,
			{ 0,0,0 },
			trans->mOffsetQuat,
			trans->mOffsetPos
		)
	);

	trans->mKeepWorldMat = RDx12::MathHelper::MulMat(newOffSetMat, RDx12::MathHelper::InverMat(SRTMat));
	RDx12::MathHelper::ShowMat(trans->mKeepWorldMat);

	//Scale������ʱ��֪����ô����
	child->UpdateWorldTranByOffset();
	return true;
}

bool Scene::RemoveNode(std::shared_ptr<SceneNode> nd)
{
	if (nd == nullptr)
		return false;

	/*
	�������½ṹ
	   A
	 B   C
	D E F G

	��Ϊ��shared_ptr ,��Ҫ�Զ��ͷ�B����Ҫ��ָ��B��ָ������ȫ��ɾ����
	1.A��child ָ��B
	2.B������child������D,E;   D E��parentָ��B
	����Ҫɾ��3��ָ��
	*/

	//���ڵ�ָ��ɾ��
	if (nd->parent != nullptr)
	{
		nd->parent->child.remove(nd);
	}

	//�ӽڵ�ָ��ɾ������ǰ�ڵ���ӽڵ�ָ��ǰ�ڵ�ĸ��ڵ�
	for (auto& child : nd->child)
	{
		child->parent = nd->parent;
		if (nd->parent != nullptr)
			nd->parent->child.push_back(child);
	}

	//�����ǰ�ڵ���ӽڵ�����
	nd->child.clear();
}

void Scene::DoForAllChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func)
{
	for (auto& child: nd->child)
	{
		func(child);
		DoForAllChild(child, func);
	}
}

void Scene::DoForNdAndAllChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func)
{
	func(nd);
	DoForAllChild(nd, func);
}

void Scene::DoForLayerChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func)
{
	for (auto& child : nd->child)
	{
		func(child);
	}
}

void Scene::DoForNdAndLayerChild(std::shared_ptr<SceneNode> nd, std::function<void(std::shared_ptr<SceneNode>)> func)
{
	func(nd);
	DoForLayerChild(nd,func);
}

std::vector<std::shared_ptr<SceneNode>> Scene::GetNodeVec(std::shared_ptr<SceneNode> nd, std::function<bool(std::shared_ptr<SceneNode>)> func)
{
	std::vector<std::shared_ptr<SceneNode>> ndVec;
	DoForAllChild(
		nd,
		[&ndVec,&func](std::shared_ptr<SceneNode> nd)
		{
			if (func(nd))
			{
				ndVec.push_back(nd);
			}
		}
	);
	return ndVec;
}

void Scene::ShowHierarchy(std::shared_ptr<SceneNode> nd,int depth=0)
{
	if (nd != nullptr)
	{
		std::string prefix;
		for (int i = 0; i < depth; i++)
		{
			prefix += "    ";
		}
		prefix += nd->name + "   type: " + nd->GetCmptStr();
		std::cout << prefix << std::endl;
		depth++;
		for (auto& child : nd->child)
		{
			ShowHierarchy(child, depth);
		}
	}
}

/*
return -1: Not find nd1;
return -2: Not find nd2;
return -3: Not find nd1 and nd2;
return 0: Not same route
return 1: Same route,nd1->nd2
return 2: Same route,nd2->nd1
*/
int Scene::IsSameRoute(std::shared_ptr<SceneNode> nd1, std::shared_ptr<SceneNode> nd2, std::shared_ptr<SceneNode> root)
{
	int timeCount = 0;
	int inTimeNd1=0, outTimeNd1 = 0;
	int inTimeNd2=0, outTimeNd2 = 0;
	IsSameRouteRcs(timeCount, inTimeNd1, outTimeNd1, inTimeNd2, outTimeNd2, nd1, nd2, root);

	if(inTimeNd1==0 && outTimeNd1 ==0 && inTimeNd2!=0 && outTimeNd2!=0)
	{
		return -1;
	}
	else if (inTimeNd1 != 0 && outTimeNd1 != 0 && inTimeNd2 == 0 && outTimeNd2 == 0)
	{
		return -2;
	}
	else if (inTimeNd1 == 0 && outTimeNd1 == 0 && inTimeNd2 == 0 && outTimeNd2 == 0)
	{
		return -3;
	}
	else
	{
		if (inTimeNd1<inTimeNd2 && outTimeNd1>outTimeNd2)
		{
			return 1;
		}
		else if (inTimeNd2<inTimeNd1 && outTimeNd2>outTimeNd1)
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
}

void Scene::IsSameRouteRcs(int& timeCount, int& inTimeNd1, int& outTimeNd1, int& inTimeNd2, int& outTimeNd2, std::shared_ptr<SceneNode> nd1, std::shared_ptr<SceneNode> nd2, std::shared_ptr<SceneNode> nowNd)
{
	timeCount += 1;
	if (nowNd == nd1)
	{
		inTimeNd1 = timeCount;
	}
	if (nowNd == nd2)
	{
		inTimeNd2 = timeCount;
	}

	for (auto child : nowNd->child)
	{
		IsSameRouteRcs(timeCount, inTimeNd1, outTimeNd1, inTimeNd2, outTimeNd2, nd1, nd2, child);
	}
	
	timeCount += 1;
	if (nowNd == nd1)
	{
		outTimeNd1 = timeCount;
	}
	if (nowNd == nd2)
	{
		outTimeNd2 = timeCount;
	}
}