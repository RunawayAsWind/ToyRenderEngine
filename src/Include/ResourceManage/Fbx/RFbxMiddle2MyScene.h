#pragma once
#include "RFbxMiddle.h"
#include "../../FrameWork_Release/TestScene001_Base.h"

class RFbxMiddle2MyScene
{
public:
	static void Add2NowScene(TestScene001_Base* testScene, RFbxMidData& midData);
private:
	static std::shared_ptr<SceneNode> MiddleNode2SceneNode(std::shared_ptr<SceneNode> fatherNode, RFbxNode& rNode, RFbxMidData& fbxMiddle, TestScene001_Base* testScene);
	static Cmpt_Trans GetTrans(RFbxNode& rNode, std::shared_ptr<SceneNode> fatherNode, bool yUporzUp);
};
