#include "../../../Include/ResourceManage/Bin/SaveLoad.h"

void SaveLoad::SaveScene(TestScene001_Base* base)
{
	auto pathVec = RWin32::Dialog_SaveFile();

	if (pathVec.size() != 0)
	{
		neb::CJsonObject sceneJson;
		/*��¼ȫ����Ϣ*/
		/*������Ϣ*/
		sceneJson.AddEmptySubArray("ObjTex");
		SaveJsonObjTex(sceneJson["ObjTex"], base);
		sceneJson.AddEmptySubArray("Node");
		SaveJsonNode(sceneJson["Node"], base->sc.root);

		std::cout << "\nSave Scene" << std::endl;
		std::cout << sceneJson.ToFormattedString() << std::endl;
		std::cout << std::endl;

		//д��������ļ�
		std::ofstream outFile(pathVec, std::ios::out | std::ios::binary);
		if (outFile.is_open())
		{
			RWin32::Bin_WriteStr(outFile, sceneJson.ToString());
			for (auto& childNode : base->sc.root->child)
			{
				SaveBinNode(childNode, outFile);
			}
			base->sc.RSerialize_Save(outFile);
		}
		outFile.close();
	}
}

void SaveLoad::LoadScene(TestScene001_Base* base)
{
	auto pathVec = RWin32::Dialog_OpenFile("FBX Files(*.data)\0*.data\0", false);

	if (pathVec.size() != 0)
	{
		//��ȡ�������ļ�
		std::ifstream inFile(pathVec[0].c_str(), std::ios::in | std::ios::binary);
		if (inFile.is_open())
		{
			/*��ȡ����json*/
			neb::CJsonObject sceneJson(RWin32::Bin_LoadStr(inFile));
			std::cout << "\nLoad Scene" << std::endl;
			std::cout << sceneJson.ToFormattedString() << std::endl;
			std::cout << std::endl;

			/*���ݳ���json��objTex�������ɲ���*/
			//��յ�ǰ���ʣ������׸����ʣ�һ����Ĭ�ϲ��ʣ���������ȫ���Ƴ�
			int totalSz = base->sdMg.sd_ObjTexTabel.GetObjTexNum();
			for (int i = 1; i < totalSz; i++)
			{
				base->sdMg.sd_ObjTexTabel.RemoveObjTex(1);
			}
			for (int i = 1; i < sceneJson["ObjTex"].GetArraySize(); i++)
			{
				neb::CJsonObject objTexJson = sceneJson["ObjTex"][i];
				std::vector<RDx12::LdTex*> ldTexVec;
				for (int j = 0; j < objTexJson["Path"].GetArraySize(); j++)
				{
					ldTexVec.push_back(&base->ldTexDic[objTexJson["Path"](j)]);
				}

				base->sdMg.sd_ObjTexTabel.AddObjTex(
					objTexJson("Name"),
					ldTexVec,
					base->dx12Base.GetDevice());
			}

			std::cout << "InitSceneNode\n" << std::endl;
			/*���ݳ���json��node�������ɳ����ڵ�*/
			//��յ�ǰ����
			base->ClearAllNode();
			neb::CJsonObject rootNodeJson = sceneJson["Node"][0];//��ȡ���ڵ�Json
			//�������ɳ����ڵ�
			for (int i = 0; i < rootNodeJson["Child"].GetArraySize(); i++)
			{
				LoadBinNode(base->sc.root, rootNodeJson["Child"][i], base,inFile);
			}
			std::cout << std::endl;


			/*��ȡmeshres����*/
			base->sc.RSerialize_Load(inFile);
			
		}
		inFile.close();
	}
}

void SaveLoad::SaveJsonObjTex(neb::CJsonObject& arrayJson, TestScene001_Base* base)
{
	for (int i = 0; i < base->sdMg.sd_ObjTexTabel.GetObjTexNum(); i++)
	{
		neb::CJsonObject objTexJson;
		objTexJson.Add("Name", *base->sdMg.sd_ObjTexTabel.GetObjTexName(i));
		objTexJson.AddEmptySubArray("Path");

		std::vector<std::string> vecPath = base->sdMg.sd_ObjTexTabel.GetObjTexPath(i);
		for (auto& path : vecPath)
		{
			objTexJson["Path"].Add(path);
		}
		arrayJson.Add(objTexJson);
	}
}

void SaveLoad::SaveJsonNode(neb::CJsonObject& arrayJson, std::shared_ptr<SceneNode> nd)
{
	if (nd != nullptr)
	{
		neb::CJsonObject ndJson;
		ndJson.Add("Name", nd->name);
		ndJson.AddEmptySubArray("Child");
		ndJson.AddEmptySubObject("Cmpt");
		CmptSave::SaveJsonCmpt<0, Cmpt_Trans, Cmpt_Camera, Cmpt_Mesh, Cmpt_ObjCb, Cmpt_PassCb,Cmpt_SkinMesh, Cmpt_SkinRoot, Cmpt_CmdParser>(nd, ndJson["Cmpt"]);
		for (auto& child : nd->child)
		{
			SaveJsonNode(ndJson["Child"], child);
		}
		arrayJson.Add(ndJson);
	}
}

void SaveLoad::SaveBinNode(std::shared_ptr<SceneNode> nd, std::ofstream& outFile)
{
	if (nd != nullptr)
	{
		CmptSave::SaveBinCmpt<0, Cmpt_Trans, Cmpt_Camera, Cmpt_Mesh, Cmpt_ObjCb, Cmpt_PassCb, Cmpt_SkinMesh, Cmpt_SkinRoot, Cmpt_CmdParser>(nd, outFile);
		for (auto& child : nd->child)
		{
			SaveBinNode(child, outFile);
		}
	}
}

void SaveLoad::LoadBinNode(std::shared_ptr<SceneNode> parent, neb::CJsonObject& ndJson, TestScene001_Base* base, std::ifstream& inFile)
{
	std::cout << "Node: " + ndJson("Name") << std::endl;
	auto newNode = std::make_shared<SceneNode>(ndJson("Name"), base->world);
	CmptSave::LoadBinCmpt<0, Cmpt_Trans, Cmpt_Camera, Cmpt_Mesh, Cmpt_ObjCb, Cmpt_PassCb, Cmpt_SkinMesh, Cmpt_SkinRoot, Cmpt_CmdParser>(newNode, inFile, ndJson["Cmpt"],base);
	Scene::AddChild(parent, newNode);

	for (int i = 0; i < ndJson["Child"].GetArraySize(); i++)
	{
		LoadBinNode(newNode, ndJson["Child"][i], base,inFile);
	}
}