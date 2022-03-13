#include "../../Include/FrameWork_Release/TestScene001_Base.h"
#include "../../Include/FrameWork_Release/Ui_CmptParser.h"
#include "../../Include/ResourceManage/Fbx/RFbxMiddle2MyScene.h"
#include "../../Include/ResourceManage/Bin/SaveLoad.h"
#include "../../Global/GobalPath.h"

void TestScene001_Base::Ui_SetDragFilePath(std::string path)
{
	qtDragFilePath = path;
}

void TestScene001_Base::UpdateByMessage()
{
    while(messageQue.size() != 0)
    {
        messageQue.front()(this);
        messageQue.pop();
    }
}

void TestScene001_Base::Set_ShowChildWin()
{
    imMain.showOtherWin = [this]()
    {
        imCmpt.ShowWin();
        imTimer.ShowWin();
        imMaterial.ShowWin();
    };

    /*材质增减界面*/
    imMaterial.updateUi = [this]()
    {
        //ObjTex可选项
        const char* items_ObjTexName[100];
        int objTexNum = sdMg.sd_ObjTexTabel.GetObjTexNum();
        for (int i = 0; i < objTexNum; i++)
        {
            items_ObjTexName[i] = sdMg.sd_ObjTexTabel.GetObjTexName(i)->c_str();
        }

        //ObjTex选择栏
        static int choose_objTexIdx = 0;
        if (ImGui::Combo("ObjTexIdx", &choose_objTexIdx, items_ObjTexName, objTexNum)) {}
        std::string qtDragTexName;	//允许Qt Project界面拖拽图片到选择框直接赋值
        if (qtDragFilePath != "")	//如果由拖拽路径数据，将其消去其前缀
        {
            qtDragTexName = RTmpl::StrReplaceAll(qtDragFilePath, GobalPath().TexturePath + "/", std::string(""));
        }

        //LdTex可选项
        const char* items_ldTexName[300];
        //选择结果数组
        static std::vector<int> choose_ldTexIdx;
        std::vector<RDx12::LdTex*> vecLd;
        choose_ldTexIdx.resize(Navigation_ObjTexCb.size());//objTex总共由八个hCpu
        vecLd.resize(Navigation_ObjTexCb.size());
        int ldTexCount = 0;//由于ldTex数据源是字典，用计数器来决定每个ldTex的下标
        int qtDragTexIdx = -1;
        for (auto& kv : ldTexDic)
        {
            items_ldTexName[ldTexCount] = kv.first.c_str();
            for (int i = 0; i < choose_ldTexIdx.size(); i++)
            {
                int objTexType = i;
                if (sdMg.sd_ObjTexTabel.GetMaphCpu(choose_objTexIdx, objTexType).ptr == kv.second.hCpu().ptr)
                {
                    choose_ldTexIdx[i] = ldTexCount;
                    vecLd[i] = &ldTexDic[items_ldTexName[choose_ldTexIdx[i]]];
                }
            }
            if (kv.first == qtDragTexName)//顺势记录Qt 拖拽路径对应的ldTex下标
            {
                qtDragTexIdx = ldTexCount;
            }
            ldTexCount++;
        }

        //ObjTex具体参数选择栏
        bool dirty = false;
        for (int i = 0; i < choose_ldTexIdx.size(); i++)
        {
            if (ImGui::Combo(Navigation_ObjTexCb[i].c_str(), &choose_ldTexIdx[i], items_ldTexName, ldTexCount))
            {
                vecLd[i] = &ldTexDic[items_ldTexName[choose_ldTexIdx[i]]];
                dirty = true;
            }
            //允许从Qt上拖拽纹理赋值
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && inputDetect.MouseUp(RDx12::InputMouse::IM_LEFT) && qtDragTexIdx != -1)
            {
                std::cout << "GetDragFile: " << qtDragTexName << "\n" << std::endl;
                choose_ldTexIdx[i] = qtDragTexIdx;
                vecLd[i] = &ldTexDic[qtDragTexName];
                dirty = true;
                qtDragFilePath = "";
            }
        }
        if (dirty == true)
        {
            sdMg.sd_ObjTexTabel.ModifyObjTex(
                choose_objTexIdx,
                *sdMg.sd_ObjTexTabel.GetObjTexName(choose_objTexIdx),
                vecLd,
                dx12Base.GetDevice()
            );
        }

        // Always center this window when appearing
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        /*Add ObjTex*/
        if (ImGui::Button("Add.."))
            ImGui::OpenPopup("Add?");
        if (ImGui::BeginPopupModal("Add?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Please Input new ObjTex's Name!\n\n");
            ImGui::Separator();

            static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                sdMg.sd_ObjTexTabel.AddObjTex(
                    newName, vecLd,
                    dx12Base.GetDevice());
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
                sdMg.sd_ObjTexTabel.ModifyObjTex(
                    choose_objTexIdx,
                    newName,
                    vecLd,
                    dx12Base.GetDevice()
                );
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
                if (choose_objTexIdx != 0)
                {
                    Scene::DoForAllChild(
                        sc.root,
                        [](std::shared_ptr<SceneNode> nd)
                        {
                            Cmpt_ObjCb* cmpt_ObjCb = nd->Get<Cmpt_ObjCb>();
                            int gObjTexCbIdx = cmpt_ObjCb->mObjCb.data.gObjTexCbIdx;
                            if (cmpt_ObjCb != nullptr)
                            {
                                if (gObjTexCbIdx >= choose_objTexIdx)
                                {
                                    gObjTexCbIdx--;
                                    cmpt_ObjCb->MustDirty();
                                }
                                else if (gObjTexCbIdx == choose_objTexIdx)
                                {
                                    gObjTexCbIdx = 0;
                                    cmpt_ObjCb->MustDirty();
                                }
                            }
                            cmpt_ObjCb->mObjCb.data.gObjTexCbIdx = gObjTexCbIdx;
                        }
                    );
                    sdMg.sd_ObjTexTabel.RemoveObjTex(choose_objTexIdx);
                    choose_objTexIdx--;
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
    };

    //从gameTimer的costTime字典中获取时间曲线
    imTimer.vecCurve.emplace_back(RImgui::CurveDraw("Update", gameTimer.costTimeDic["Update"], 0, 100));
    imTimer.vecCurve.emplace_back(RImgui::CurveDraw("Render", gameTimer.costTimeDic["Render"], 0, 100));
}

void TestScene001_Base::Set_ShowMainWinUi()
{
    imMain.showMainWinUi = [this]()
    {
        if (ImGui::BeginMenuBar())
        {

            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::MenuItem("(dummy menu)", NULL, false, false);
                if (ImGui::MenuItem("New"))
                {
                    imHierachy.ClearUi();
                    imCmpt.ClearUi();//Ui函数保存了nd引用，imMain.imCmpt.updateUi_NodeCmpt需要重新赋值，才能保证节点在删除后立即释放
                    this->imCmpt.show = false;//更保险一点关掉节点相关页面
                    ClearAllNode();
                }
                if (ImGui::MenuItem("Save"))
                {
                    SaveLoad::SaveScene(this);
                }
                if (ImGui::MenuItem("Load"))
                {
                    SaveLoad::LoadScene(this);
                    layerDirty = true;
                    meshDirty = true;
                }
                if (ImGui::BeginMenu("Import"))
                {
                    if (ImGui::MenuItem(".fbx"))
                    {
                        //1.先进行文件选取
                        auto pathVec = RWin32::Dialog_OpenFile("FBX Files(*.fbx)\0*.fbx\0", false);

                        //2.如果选中了文件，对选中的fbx文件进行处理
                        if (pathVec.size() != 0)
                        {
                            RFbxMiddle2MyScene::Add2NowScene(this, RFbxMiddle::GetMidData(pathVec[0]));
                            layerDirty = true;
                            meshDirty = true;
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("AddMaterial"))
                {
                    if (ImGui::MenuItem("ObjTex"))
                    {
                        imMaterial.show = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Examples"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Timer"))
                {
                    imTimer.show = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("VertexNum %d IndexNum %d", vertexNum, indexNum);
        ImGui::Text("SkinVertexNum %d SkinIndexNum %d", skinVertexNum, skinIndexNum);
        ImGui::Spacing();

        imHierachy.root = sc.root;

        //点击节点处理函数
        imHierachy.clickFunc = [this](std::shared_ptr<SceneNode> nd)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.KeyCtrl && ImGui::IsItemClicked(0))
            {
                nd->ChangeSelect(nd->is_selected);
                layerDirty = true;
            }
            else if (io.KeyAlt && ImGui::IsItemClicked(0))
            {
                Scene::DoForNdAndAllChild(nd, [](std::shared_ptr<SceneNode> nd) {nd->ChangeSelect(true); });
                layerDirty = true;
            }
            else if (io.KeyShift && ImGui::IsItemClicked(0))
            {
                Scene::DoForNdAndLayerChild(nd, [](std::shared_ptr<SceneNode> nd) {nd->ChangeSelect(true); });
                layerDirty = true;
            }
            else if (ImGui::IsItemClicked(0))
            {
                Scene::DoForNdAndAllChild(sc.root, [](std::shared_ptr<SceneNode> nd) {nd->ChangeSelect(false); });
                nd->ChangeSelect(true);
                layerDirty = true;

                std::cout << nd->name << " " << nd->GetArch()->cmptsRef.GetCmptMetaVec().size() << std::endl;

                //点击一个节点时，如果有上一次点击的组件类型，则显示相同类型的组件属性面板，否则关闭上一点击节点组件的属性面板
                this->imCmpt.show = true;

                //注意这里“Ui函数保存了nd引用”，当删除nd的时候需要重新赋值lambda清除引用
                imCmpt.hasNodeMap.clear();
                Ui_CmptParser::InitParserBtn<0, Cmpt_Trans, Cmpt_Camera, Cmpt_Mesh, Cmpt_ObjCb, Cmpt_PassCb, Cmpt_SkinMesh, Cmpt_SkinRoot, Cmpt_CmdParser>(imCmpt.hasNodeMap, nd, this);

                /*组件界面的可选择组件*/
                //注意这里“Ui函数保存了nd引用”，当删除nd的时候需要重新赋值lambda清除引用
                imCmpt.chooseNodeMap.clear();
                Ui_CmptParser::InitChooseBtn<0, Cmpt_Trans, Cmpt_Camera, Cmpt_Mesh, Cmpt_ObjCb, Cmpt_PassCb, Cmpt_SkinMesh, Cmpt_SkinRoot, Cmpt_CmdParser>(imCmpt.chooseNodeMap, nd, this);
            }
        };
        
        //右击节点出现窗口
        imHierachy.rightClickUi = [this](std::shared_ptr<SceneNode> nd)
        {
            ImGui::Text("Please Input new Node Name!\n\n");
            ImGui::Separator();

            static char newName[64] = ""; ImGui::InputText("Name", newName, 64, ImGuiInputTextFlags_CharsNoBlank);

            if (ImGui::Button("Modify", ImVec2(120, 0)))
            {
                nd->name = newName;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                auto newNode = std::make_shared<SceneNode>(newName, this->world);
                Scene::AddChild(nd, newNode);
                ImGui::CloseCurrentPopup();
            }
        };

        //拖动节点处理函数
        imHierachy.dragEndFunc = [](std::shared_ptr<SceneNode> dragNode, std::shared_ptr<SceneNode> endNode)
        {
            Scene::AddChildKeepWorldMat(endNode, dragNode);
        };
    
        imHierachy.colDrawFuncs.clear();
        /*层次结构节点右侧每列的Text*/
        imHierachy.colDrawFuncs.push_back(
            [](std::shared_ptr<SceneNode> nd) {
                ImGui::NextColumn();
                ImGui::Text(nd->GetCmptStr().c_str());
            }
        );

        static bool showDeleteNode = false;
        if (inputDetect.KeyDown(RDx12::InputKey::IK_DELETE))
        {
            std::vector<std::shared_ptr<SceneNode>> delVec;
            sc.DoForAllChild(
                sc.root,
                [&delVec](std::shared_ptr<SceneNode> nd)
                {
                    if (nd->is_selected == true)
                        delVec.push_back(nd);
                }
            );
            if (delVec.size() != 0)
            {
                showDeleteNode = true;
            }
        }
        if (showDeleteNode == true)
        {
            // Always center this window when appearing
            ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            ImGui::OpenPopup("Remove?");
            if (ImGui::BeginPopupModal("Remove?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Do you really want to remove?\n\n");
                ImGui::Separator();

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    imHierachy.ClearUi();
                    imCmpt.ClearUi();//Ui函数保存了nd引用，imMain.imCmpt.updateUi_NodeCmpt需要重新赋值，才能保证节点在删除后立即释放
                    imCmpt.show = false;//更保险一点关掉节点相关页面

                    showDeleteNode = false;
                    std::vector<std::shared_ptr<SceneNode>> delVec;
                    sc.DoForAllChild(
                        sc.root,
                        [&delVec](std::shared_ptr<SceneNode> nd)
                        {
                            if (nd->is_selected == true)
                                delVec.push_back(nd);
                        }
                    );
                    for (auto delNd : delVec)
                    {
                        std::cout << delNd->name << std::endl;
                        Scene::RemoveNode(delNd);
                        DeleteFromAllLayer(delNd);
                        layerDirty = true;
                    }
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); showDeleteNode = false; }
                ImGui::EndPopup();
            }
        }

        imHierachy.Show();
    };
}