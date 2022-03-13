#pragma once
#include "../../FrameWork_Release/TestScene001_Base.h"
#include "../Json/CJsonObject.hpp"

class CmptSave
{
public:
	/*模板递归进行组件属性记录*/
	template<size_t id, typename HeadCmpt, typename...RestCmpt>
	static void SaveJsonCmpt(std::shared_ptr<SceneNode> nd, neb::CJsonObject& arrayJson)
	{
		if (nd->Get<HeadCmpt>() != nullptr)
		{
			arrayJson.AddEmptySubObject(RTmpl::Type_SimpleName<HeadCmpt>());
		}

		SaveJsonCmpt<id + 1, RestCmpt...>(nd, arrayJson);
	};

	/*终止函数*/
	template<size_t id>
	static void SaveJsonCmpt(std::shared_ptr<SceneNode> nd, neb::CJsonObject& arrayJson) {};


	/*模板递归进行组件属性记录*/
	template<size_t id, typename HeadCmpt, typename...RestCmpt>
	static void SaveBinCmpt(std::shared_ptr<SceneNode> nd, std::ofstream& outFile)
	{
		if (nd->Get<HeadCmpt>() != nullptr)
		{
			nd->Get<HeadCmpt>()->RSerialize_Save(outFile);
		}

		SaveBinCmpt<id + 1, RestCmpt...>(nd, outFile);
	};

	/*终止函数*/
	template<size_t id>
	static void SaveBinCmpt(std::shared_ptr<SceneNode> nd, std::ofstream& outFile) {};

	/*模板递归进行组件属性记录*/
	template<size_t id, typename HeadCmpt, typename...RestCmpt>
	static void LoadBinCmpt(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, neb::CJsonObject& cmptJson, TestScene001_Base* base)
	{
		if (cmptJson.KeyExist(RTmpl::Type_SimpleName<HeadCmpt>()))
		{
			LoadBinAndInitCmpt<HeadCmpt>(nd, inFile, base);
		}

		LoadBinCmpt<id + 1, RestCmpt...>(nd, inFile, cmptJson,base);
	};

	/*终止函数*/
	template<size_t id>
	static void  LoadBinCmpt(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, neb::CJsonObject& cmptJson, TestScene001_Base* base){};

	template<typename Cmpt>
	static void LoadBinAndInitCmpt(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, TestScene001_Base* base)
	{
		nd->Add<Cmpt>();
		nd->Get<Cmpt>()->RSerialize_Load(inFile);
	}

	template<>
	static void LoadBinAndInitCmpt<Cmpt_PassCb>(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, TestScene001_Base* base)
	{
		nd->Add<Cmpt_PassCb>();
		nd->Get<Cmpt_PassCb>()->AllocRes(base->sdMg.sd_PassCb);
		nd->Get<Cmpt_PassCb>()->RSerialize_Load(inFile);
	}

	template<>
	static void LoadBinAndInitCmpt<Cmpt_ObjCb>(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, TestScene001_Base* base)
	{
		nd->Add<Cmpt_ObjCb>();
		nd->Get<Cmpt_ObjCb>()->AllocRes(base->sdMg.sd_ObjCb);
		nd->Get<Cmpt_ObjCb>()->RSerialize_Load(inFile);
	}

	template<>
	static void LoadBinAndInitCmpt<Cmpt_SkinMesh>(std::shared_ptr<SceneNode> nd, std::ifstream& inFile, TestScene001_Base* base)
	{
		nd->Add<Cmpt_SkinMesh>();
		nd->Get<Cmpt_SkinMesh>()->AllocRes(base->sdMg.sd_SkinnedCb);
		nd->Get<Cmpt_SkinMesh>()->RSerialize_Load(inFile);
	}
};