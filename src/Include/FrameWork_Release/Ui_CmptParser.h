#pragma once
#include <RImgui/ImMain.h>
#include "../../Include/FrameWork_Release/TestScene001_Base.h"

class Ui_CmptParser
{
public:
	/*模板递归生成可选择的组件列表*/
	template<size_t id, typename HeadCmpt, typename...RestCmpt>
	static void InitChooseBtn(std::map<std::string, std::function<void()>>& btnMap,std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		std::string btnName = RTmpl::Type_SimpleName<HeadCmpt>();

		btnMap[btnName] = [nd, base]() {};
		if (btnName == RTmpl::Type_SimpleName<Cmpt_Mesh>())
		{
			btnMap[btnName] = [nd, base]() { base->SetAsRenderItem(nd, 2, RenderLayer::NonePbr, 0); };
		}
		if (btnName == RTmpl::Type_SimpleName<Cmpt_Camera>())
		{
			btnMap[btnName] = [nd, base]() { base->SetAsCamera(nd, CameraLayer::Main); };
		}
		if (btnName == RTmpl::Type_SimpleName<Cmpt_SkinRoot>())
		{
			btnMap[btnName] = [nd, base]() { base->SetAsSkinRoot(nd, 100); };
		}
		if (btnName == RTmpl::Type_SimpleName<Cmpt_CmdParser>())
		{
			btnMap[btnName] = [nd, base]() {nd->Add<Cmpt_CmdParser>(); };
		}

		
		InitChooseBtn<id + 1, RestCmpt...>(btnMap,nd,base);
	};

	/*终止函数*/
	template<size_t id>
	static void InitChooseBtn(std::map<std::string, std::function<void()>>& btnMap,std::shared_ptr<SceneNode> nd, TestScene001_Base* base) {  };



    //根据当前节点所含组件生成按钮，点击按钮可显示组件属性
    template<size_t id, typename HeadCmpt, typename...RestCmpt>
    static void InitParserBtn(std::map<std::string, std::function<void()>>& btnMap,std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
    {
		std::string btnName = RTmpl::Type_SimpleName<HeadCmpt>();

        /*如果有组件，生成Cmpt的Inspect按钮，点击后解码组件，显示Inspect界面*/
        if (nd->Get<HeadCmpt>() != nullptr)
        {
			btnMap[btnName] = [nd, base]() {
				ParserCmpt<HeadCmpt>(nd, base);
				base->imCmpt.lastChooseCmptId = RTmpl::Type_SimpleName<HeadCmpt>();
			};

            InitParserBtn<id + 1, RestCmpt...>(btnMap,nd, base);
        }
        else//无该组件则跳过
        {
            InitParserBtn<id, RestCmpt...>(btnMap,nd, base);
        }
    }

    /*终止函数*/
    template<size_t id>
    static void InitParserBtn(std::map<std::string, std::function<void()>>& btnMap,std::shared_ptr<SceneNode> nd, TestScene001_Base* base) {};

	//组件解码
    template<typename Cmpt>
    static void ParserCmpt(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
    {
        bool cmptDataDirty = false;
		Cmpt* cmpt = nd->Get<Cmpt>();
		//遍历组件的每一个成员变量，生成相应的控件
        RTmpl::Reflection<Cmpt>::DoForEachVal(
            [cmpt,&cmptDataDirty, base](auto field)
            {
                auto val = &(cmpt->*field.ptr);
                cmptDataDirty |= ParserData(field, base,val);//通过imgui来判断组件数据是否被改变
            }
        );
        if (cmptDataDirty){
            HandleDirty(cmpt);
        }
    }

	//特化模板实现部分特殊组件的解码
	template<>
	static void ParserCmpt<Cmpt_Trans>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		DirectX::XMFLOAT3 dPos(0, 0, 0);
		DirectX::XMFLOAT3 dScale(1, 1, 1);
		DirectX::XMFLOAT4 dQuat(0, 0, 0, 1);
		DirectX::XMFLOAT3 dEuler(0, 0, 0);

		ImGui::PushItemWidth(150);
		Cmpt_Trans* cmpt = nd->Get<Cmpt_Trans>();

		DirectX::XMFLOAT3 oldPos = cmpt->mPos;
		DirectX::XMFLOAT3 newPos = cmpt->mPos;
		ImGui::Text("Pos:    "); ImGui::SameLine();
		bool b1 = ImGui::InputFloat_DIY("PosX","X:", &newPos.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		bool b2 = ImGui::InputFloat_DIY("PosY","Y:", &newPos.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		bool b3 = ImGui::InputFloat_DIY("PosZ","Z:", &newPos.z, 0.1f, 1.0f, "%.3f");
		if (b1|b2|b3)
		{
			dPos = DirectX::XMFLOAT3(newPos.x - oldPos.x, newPos.y - oldPos.y, newPos.z - oldPos.z);
		}

		DirectX::XMFLOAT3 oldScale = cmpt->mOffsetScale;
		DirectX::XMFLOAT3 newScale = cmpt->mOffsetScale;
		ImGui::Text("Scale:  ");ImGui::SameLine();
		bool b4 = ImGui::InputFloat_DIY("ScaleX", "X:", &newScale.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		bool b5 = ImGui::InputFloat_DIY("ScaleY", "Y:", &newScale.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		bool b6 = ImGui::InputFloat_DIY("ScaleZ", "Z:", &newScale.z, 0.1f, 1.0f, "%.3f");
		if (b4 | b5 | b6)
		{
			dScale = DirectX::XMFLOAT3(newScale.x / oldScale.x, newScale.y / oldScale.y, newScale.z / oldScale.z);
		}

		static DirectX::XMFLOAT3 wPos{ 0,0,0 };
		static DirectX::XMFLOAT3 wScale{ 1,1,1 };
		static DirectX::XMFLOAT3 wEuler{ 0,0,0 };

		ImGui::Text("wPos:   "); ImGui::SameLine();
		ImGui::InputFloat_DIY("wPosX", "X:", &wPos.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wPosY", "Y:", &wPos.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wPosZ", "Z:", &wPos.z, 0.1f, 1.0f, "%.3f");
		ImGui::Text("wScale: "); ImGui::SameLine();
		ImGui::InputFloat_DIY("wScaleX", "X:", &wScale.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wScaleY", "Y:", &wScale.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wScaleZ", "Z:", &wScale.z, 0.1f, 1.0f, "%.3f");
		ImGui::Text("wEuler: "); ImGui::SameLine();
		ImGui::InputFloat_DIY("wEulerX", "X:", &wEuler.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wEulerY", "Y:", &wEuler.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("wEulerZ", "Z:", &wEuler.z, 0.1f, 1.0f, "%.3f");

		ImGui::PopItemWidth();

		static float rotDegSpeed = 0.5;
		ImGui::InputFloat_DIY("RotDegSpeed", "RotDegSpeed:", &rotDegSpeed, 0.1f, 1.0f, "%.3f");

		ImGui::PushButtonRepeat(true);
		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::AlignTextToFramePadding();

		ImGui::Text("EulerDegX: %6.1f", cmpt->mEuler.x);
		ImGui::SameLine();
		if (ImGui::ArrowButton("RotByX_Back", ImGuiDir_Left)) { dEuler.x -= rotDegSpeed; }
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("RotByX", ImGuiDir_Right)) { dEuler.x += rotDegSpeed; }

		ImGui::Text("EulerDegY: %6.1f", cmpt->mEuler.y);
		ImGui::SameLine();
		if (ImGui::ArrowButton("RotByY_Back", ImGuiDir_Left)) { dEuler.y -= rotDegSpeed; }
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("RotByY", ImGuiDir_Right)) { dEuler.y += rotDegSpeed; }

		ImGui::Text("EulerDegZ: %6.1f", cmpt->mEuler.z);
		ImGui::SameLine();
		if (ImGui::ArrowButton("RotByZ_Back", ImGuiDir_Left)) { dEuler.z-= rotDegSpeed;}
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("RotByZ", ImGuiDir_Right)) { dEuler.z += rotDegSpeed; }

		ImGui::PopButtonRepeat();

		dQuat = RDx12::MathHelper::GetRollPitchYawQuatByDeg(dEuler.x, dEuler.y, dEuler.z);

		ImGui::Text("Pos X: %6.1f", cmpt->mOffsetPos.x);ImGui::SameLine();
		ImGui::Text("Pos Y: %6.1f", cmpt->mOffsetPos.y);ImGui::SameLine();
		ImGui::Text("Pos Z: %6.1f", cmpt->mOffsetPos.z);
		
		ImGui::Text("Scale X: %6.1f", cmpt->mOffsetScale.x);ImGui::SameLine();
		ImGui::Text("Scale Y: %6.1f", cmpt->mOffsetScale.y);ImGui::SameLine();
		ImGui::Text("Scale Z: %6.1f", cmpt->mOffsetScale.z);
		
		ImGui::Text("Quat X: %6.1f", cmpt->mOffsetQuat.x);ImGui::SameLine();
		ImGui::Text("Quat Y: %6.1f", cmpt->mOffsetQuat.y);ImGui::SameLine();
		ImGui::Text("Quat Z: %6.1f", cmpt->mOffsetQuat.z);ImGui::SameLine();
		ImGui::Text("Quat W: %6.1f", cmpt->mOffsetQuat.w);

		if (!RDx12::MathHelper::F3Equal(dPos, { 0,0,0 }) || !RDx12::MathHelper::F3Equal(dScale, { 1,1,1 }) || !RDx12::MathHelper::F4Equal(dQuat, { 0,0,0,1 }))
		{
			nd->ChangeWorldTrans(dPos, dScale, dQuat);
		}

		if (RDx12::MathHelper::F4x4Equal(cmpt->mKeepWorldMat, RDx12::MathHelper::Identity4x4()))
		{
			ImGui::Text("KeepWorldMat: No");
		}
		else
		{
			ImGui::Text("KeepWorldMat: Yes");
		}
		if(ImGui::Button("Clear KeepWorldMat.."))
			ImGui::OpenPopup("Clear KeepWorldMat?");
		if (ImGui::BeginPopupModal("Clear KeepWorldMat?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				Scene::DoForNdAndAllChild(nd, [](std::shared_ptr<SceneNode> nd) { nd->Get<Cmpt_Trans>()->mKeepWorldMat = RDx12::MathHelper::Identity4x4(); });
				nd->UpdateWorldTranByOffset();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("Set WorldMat.."))
			ImGui::OpenPopup("Set WorldMat?");
		if (ImGui::BeginPopupModal("Set WorldMat?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				nd->SetWorldTrans(wPos,wScale,wEuler);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	/*    
		Cmpt_Trans* mTrans = nd->Get<Cmpt_Trans>();
		bool mDirty = false;
		if (
			ImGui::InputFloat("Pos X:", &mTrans->mPos.x, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("Pos Y:", &mTrans->mPos.y, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("Pos Z:", &mTrans->mPos.z, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("mScale X:", &mTrans->mScale.x, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("mScale Y:", &mTrans->mScale.y, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("mScale Z:", &mTrans->mScale.z, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("Euler X:", &mTrans->mEuler.x, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("Euler Y:", &mTrans->mEuler.y, 0.1f, 1.0f, "%.3f") |
			ImGui::InputFloat("Euler Z:", &mTrans->mEuler.z, 0.1f, 1.0f, "%.3f")
		)
		{
			mDirty = true;
		}

		DirectX::XMStoreFloat4(
			&mTrans->mQuat,
			DirectX::XMQuaternionRotationRollPitchYaw(
				RDx12::MathHelper::Deg2Rad(mTrans->mEuler.x),
				RDx12::MathHelper::Deg2Rad(mTrans->mEuler.y),
				RDx12::MathHelper::Deg2Rad(mTrans->mEuler.z)
			)
		);

		ImGui::Text("Pos X: %6.1f", mTrans->mOffsetPos.x);
		ImGui::SameLine();
		ImGui::Text("Pos Y: %6.1f", mTrans->mOffsetPos.y);
		ImGui::SameLine();
		ImGui::Text("Pos Z: %6.1f", mTrans->mOffsetPos.z);
		ImGui::Text("mScale X: %6.1f", mTrans->mOffsetScale.x);
		ImGui::SameLine();
		ImGui::Text("mScale Y: %6.1f", mTrans->mOffsetScale.y);
		ImGui::SameLine();
		ImGui::Text("mScale Z: %6.1f", mTrans->mOffsetScale.z);
		ImGui::Text("mQuat X: %6.1f", mTrans->mOffsetQuat.x);
		ImGui::SameLine();
		ImGui::Text("mQuat Y: %6.1f", mTrans->mOffsetQuat.y);
		ImGui::SameLine();
		ImGui::Text("mQuat Z: %6.1f", mTrans->mOffsetQuat.z);
		ImGui::SameLine();
		ImGui::Text("mQuat W: %6.1f", mTrans->mOffsetQuat.w);

		if (mDirty)
		{
			nd->SetOffsetTrans(mTrans->mPos, mTrans->mScale, mTrans->mQuat);
		}
	*/
	}

    template<>
    static void ParserCmpt<Cmpt_Mesh>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
    {
		Cmpt_Mesh* cmpt = nd->Get<Cmpt_Mesh>();

		const char* items_MeshName[100];
		int meshNum = base->sc.vecPureMesh.size();
		for (int i = 0; i < meshNum; i++)
		{
			items_MeshName[i] = base->sc.vecPureMesh[i].name.c_str();
		}
		//生成选择列表:              列表label                    列表影响的数据 列表内容 列表大小      
		if (ImGui::Combo(RTmpl::Type_SimpleName<Cmpt_Mesh>().c_str(), &cmpt->mMeshIdx, items_MeshName, meshNum)) 
		{
			base->meshDirty = true;
		}
    }

	template<>
	static void ParserCmpt<Cmpt_SkinMesh>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		Cmpt_SkinMesh* cmpt = nd->Get<Cmpt_SkinMesh>();

		const char* items_MeshName[100];
		int meshNum = base->sc.vecSkinMesh.size();
		for (int i = 0; i < meshNum; i++)
		{
			items_MeshName[i] = base->sc.vecSkinMesh[i].name.c_str();
		}
		//生成选择列表:              列表label                    列表影响的数据 列表内容 列表大小      
		if (ImGui::Combo(RTmpl::Type_SimpleName<Cmpt_SkinMesh>().c_str(), &cmpt->mSkinMeshIdx, items_MeshName, meshNum)) 
		{
			base->meshDirty = true;
		}
	}

	template<>
	static void ParserCmpt<Cmpt_SkinRoot>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		Cmpt_SkinRoot* cmpt = nd->Get<Cmpt_SkinRoot>();

		const char* items_ClipName[100];
		int clipNum = cmpt->mClipVec.size();
		for (int i = 0; i < clipNum; i++)
		{
			items_ClipName[i] = cmpt->mClipVec[i].name.c_str();
		}
		//生成选择列表:              列表label                    列表影响的数据 列表内容 列表大小      
		if (ImGui::Combo(RTmpl::Type_SimpleName<Cmpt_SkinRoot>().c_str(), &cmpt->mClipIdx, items_ClipName, clipNum))
		{
			cmpt->ChangeClip(cmpt->mClipIdx);
		}

		ImGui::InputFloat("StartTime", &cmpt->mClipVec[cmpt->mClipIdx].startTime); ImGui::InputFloat("EndTime", &cmpt->mClipVec[cmpt->mClipIdx].endTime);

		ImGui::InputFloat("TimePos:", &cmpt->mTimePos, 0.1f, 1.0f, "%.3f");

		ImGui::InputFloat("mSpeed:", &cmpt->mSpeed, 0.1f, 1.0f, "%.3f");

		/*Add Clip*/
		if (ImGui::Button("Add.."))
			ImGui::OpenPopup("Add?");
		if (ImGui::BeginPopupModal("Add?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please Input new Clip's Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				cmpt->mClipVec.push_back({ newName,0,100 });
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Modify Name*/
		if (ImGui::Button("Modify.."))
			ImGui::OpenPopup("Modify?");
		if (ImGui::BeginPopupModal("Modify?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please Input new ObjTex's Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				if (cmpt->mClipIdx != 0)
				{
					cmpt->mClipVec[cmpt->mClipIdx].name = newName;
				}
				else
				{

				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Remove*/
		if (ImGui::Button("Remove.."))
			ImGui::OpenPopup("Remove?");
		if (ImGui::BeginPopupModal("Remove?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Do you really want to remove?\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				if (cmpt->mClipIdx != 0)
				{
					cmpt->mClipVec.erase(cmpt->mClipVec.begin() + cmpt->mClipIdx);
					cmpt->ChangeClip(--cmpt->mClipIdx);
				}
				else
				{

				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

	template<>
	static void ParserCmpt<Cmpt_ObjCb>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		Cmpt_ObjCb* cmpt = nd->Get<Cmpt_ObjCb>();

		const char* items_RenderName[100];
		int renderNum = Navigation_RenderLayer.size();
		for (int i = 0; i < renderNum; i++)
		{
			items_RenderName[i] = Navigation_RenderLayer[i].c_str();
		}

		if (ImGui::Combo("RenderIdx", &cmpt->mObjCb.data.gRenderIdx, items_RenderName, renderNum))
		{
			base->layerDirty = true;
			cmpt->MustDirty();
		}

		const char* items_ObjTexName[100];
		int objTexNum = base->sdMg.sd_ObjTexTabel.GetObjTexNum();
		for (int i = 0; i < objTexNum; i++)
		{
			items_ObjTexName[i] = base->sdMg.sd_ObjTexTabel.GetObjTexName(i)->c_str();
		}

		if (ImGui::Combo("ObjTexIdx", &cmpt->mObjCb.data.gObjTexCbIdx, items_ObjTexName, objTexNum))
		{
			cmpt->MustDirty();
		}

		const char* items_SkyName[100];
		int skyNum = base->vecSkyBox.size();
		for (int i = 0; i < skyNum; i++)
		{
			items_SkyName[i] = base->vecSkyBox[i].Name();
		}
		if (ImGui::Combo("SkyIdx", &base->nowSkyBoxIdx, items_SkyName, skyNum)){}
	}

	template<>
	static void ParserCmpt<Cmpt_CmdParser>(std::shared_ptr<SceneNode> nd, TestScene001_Base* base)
	{
		Cmpt_CmdParser* cmpt = nd->Get<Cmpt_CmdParser>();

		const char* items_LogicName[100];
		int logicNum = cmpt->mLogicVec.size();
		for (int i = 0; i < logicNum; i++)
		{
			items_LogicName[i] = cmpt->mLogicVec[i].name.c_str();
		}
		//生成选择列表:              列表label                    列表影响的数据 列表内容 列表大小      
		if (ImGui::Combo(RTmpl::Type_SimpleName<Cmpt_CmdParser>().c_str(), &cmpt->mLogicIdx, items_LogicName, logicNum))
		{
			//cmpt->ChangeClip(cmpt->mLogicIdx);
		}

		//ImGui::InputFloat("StartTime", &cmpt->mClipVec[cmpt->mLogicIdx].startTime); ImGui::InputFloat("EndTime", &cmpt->mClipVec[cmpt->mLogicIdx].endTime);

		ImGui::InputFloat("TimePos:", &cmpt->mTimePos, 0.1f, 1.0f, "%.3f");

		ImGui::InputFloat("mSpeed:", &cmpt->mSpeed, 0.1f, 1.0f, "%.3f");

		/*Add Logic*/
		if (ImGui::Button("AddLogic.."))
			ImGui::OpenPopup("AddLogic");
		if (ImGui::BeginPopupModal("AddLogic", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please Input new Logic's Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				cmpt->mLogicVec.push_back({newName,{{"Default"}}});
				cmpt->mLogicIdx = cmpt->mLogicVec.size() - 1;
				cmpt->mUnitIdx = 0;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Modify Name*/
		if (ImGui::Button("ModifyLogic.."))
			ImGui::OpenPopup("ModifyLogic");
		if (ImGui::BeginPopupModal("ModifyLogic", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please Input new Logic''s Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				cmpt->mLogicVec[cmpt->mLogicIdx].name = newName;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Remove Logic*/
		if (ImGui::Button("RemoveLogic.."))
			ImGui::OpenPopup("RemoveLogic");
		if (ImGui::BeginPopupModal("RemoveLogic", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Do you really want to remove?\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				if (cmpt->mLogicIdx != 0)
				{
					cmpt->mLogicVec.erase(cmpt->mLogicVec.begin() + cmpt->mLogicIdx);
					cmpt->mLogicIdx--;
					cmpt->mUnitIdx = 0;
				}
				else
				{

				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		const char* items_AnimUnitName[100];
		int unitNum = cmpt->GetAnimUnitList().size();
		for (int i = 0; i < unitNum; i++)
		{
			items_AnimUnitName[i] = cmpt->GetAnimUnitList()[i].clipName.c_str();
		}

		//生成选择列表:              列表label                    列表影响的数据 列表内容 列表大小      
		if (ImGui::Combo("Cmpt_CmdParser_UnitList", &cmpt->mUnitIdx, items_AnimUnitName, unitNum))
		{
			//cmpt->ChangeClip(cmpt->mLogicIdx);
		}

		//ImGui::InputText("ClipName:", cmpt->GetAnimUnit().clipName.data(), cmpt->GetAnimUnit().clipName.size()+1);
		ImGui::PushItemWidth(150);
		ImGui::Text("Pos:    "); ImGui::SameLine();
		ImGui::InputFloat_DIY("PosX", "X:", &cmpt->GetAnimUnit().posOffset.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("PosY", "Y:", &cmpt->GetAnimUnit().posOffset.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("PosZ", "Z:", &cmpt->GetAnimUnit().posOffset.z, 0.1f, 1.0f, "%.3f");

		ImGui::Text("Scale:  "); ImGui::SameLine();
		ImGui::InputFloat_DIY("ScaleX", "X:", &cmpt->GetAnimUnit().scaleOffset.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("ScaleY", "Y:", &cmpt->GetAnimUnit().scaleOffset.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("ScaleZ", "Z:", &cmpt->GetAnimUnit().scaleOffset.z, 0.1f, 1.0f, "%.3f");

		ImGui::Text("Quat:   "); ImGui::SameLine();
		ImGui::InputFloat_DIY("QuatX", "X:", &cmpt->GetAnimUnit().quatOffset.x, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("QuatY", "Y:", &cmpt->GetAnimUnit().quatOffset.y, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("QuatZ", "Z:", &cmpt->GetAnimUnit().quatOffset.z, 0.1f, 1.0f, "%.3f"); ImGui::SameLine();
		ImGui::InputFloat_DIY("QuatW", "W:", &cmpt->GetAnimUnit().quatOffset.w, 0.1f, 1.0f, "%.3f");

		if (ImGui::Button("AddUnit.."))
			ImGui::OpenPopup("AddUnit");
		if (ImGui::BeginPopupModal("AddUnit", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please Input new Uint's Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				cmpt->GetAnimUnitList().push_back({newName});
				cmpt->mUnitIdx = cmpt->GetAnimUnitList().size() - 1;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Modify Name*/
		if (ImGui::Button("ModifyUnit.."))
			ImGui::OpenPopup("ModifyUnit");
		if (ImGui::BeginPopupModal("ModifyUnit", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static int unitIdx = cmpt->mUnitIdx;
			ImGui::Text("Please Input new Unit's Name!\n\n");
			ImGui::Separator();

			static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				cmpt->GetAnimUnitList()[unitIdx].clipName = newName;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		/*Remove Unit*/
		if (ImGui::Button("RemoveUnit.."))
			ImGui::OpenPopup("RemoveUnit");
		if (ImGui::BeginPopupModal("RemoveUnit", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Do you really want to remove?\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				if (cmpt->mUnitIdx != 0)
				{
					cmpt->GetAnimUnitList().erase(cmpt->GetAnimUnitList().begin() + cmpt->mUnitIdx);
					cmpt->mUnitIdx--;
					//cmpt->ChangeClip(--cmpt->mLogicIdx);
				}
				else
				{

				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

	//公共的组件解码部分
    //通过迭代表达式来处理反射域内的不同类型
    template<typename Data, typename... Args>
    static bool ParserData(RTmpl::Field<Args...> field, TestScene001_Base* base,Data* data) { return false; };

    /*处理float3*/
    template<typename... Args>
    static bool ParserData(RTmpl::Field<Args...> field, TestScene001_Base* base,DirectX::XMFLOAT3* data)
    {
        if(
            ImGui::InputFloat((field.GetName() + " X:").c_str(), &data->x, 0.1f, 1.0f, "%.3f") |
            ImGui::InputFloat((field.GetName() + " Y:").c_str(), &data->y, 0.1f, 1.0f, "%.3f") |
            ImGui::InputFloat((field.GetName() + " Z:").c_str(), &data->z, 0.1f, 1.0f, "%.3f")
          )
        { return true; }
        return false;
    }
   
    /*通过模板特化捕获具体类型进行内容改变后的处理*/
    template<typename Cmpt>
    static void HandleDirty(Cmpt* cmpt)
    {}

 
    template<>
    static void HandleDirty<Cmpt_Trans>(Cmpt_Trans* cmpt)
    {
        cmpt->MustDirty();
    }
};