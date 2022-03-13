#pragma once
#include "CmptSave.h"

class SaveLoad
{
public:
	static void SaveScene(TestScene001_Base* base);
	static void LoadScene(TestScene001_Base* base);
private:
	static void SaveJsonObjTex(neb::CJsonObject& arrayJson, TestScene001_Base* base);
	static void SaveJsonNode(neb::CJsonObject& arrayJson, std::shared_ptr<SceneNode> nd);
	static void SaveBinNode(std::shared_ptr<SceneNode> nd, std::ofstream& outFile);
	static void LoadBinNode(std::shared_ptr<SceneNode> parent, neb::CJsonObject& ndJson, TestScene001_Base* base,std::ifstream& inFile);
};