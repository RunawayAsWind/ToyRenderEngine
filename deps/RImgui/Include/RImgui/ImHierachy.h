#pragma once
#include "../../3rdParty/Imgui/imgui.h"
#include <memory>
#include<iostream>
namespace RImgui
{
	/*
		class NodeType
		{
			public:
			std::string name;
			bool is_selected=false;
		}
	*/
	template<typename NodeType>
	class ImHierachy
	{
	public:
		enum class NodeOption
		{
			Open = 1,
			Close
		};

		bool show = false;
		std::shared_ptr<NodeType> dragNode = nullptr;
		std::shared_ptr<NodeType> root = nullptr;
		std::function<void(std::shared_ptr<NodeType>)> clickFunc = [](std::shared_ptr<NodeType> nd) {};
		std::function<void(std::shared_ptr<NodeType>)> rightClickUi = [](std::shared_ptr<NodeType> nd) {};
		std::function<void(std::shared_ptr<NodeType>, std::shared_ptr<NodeType>)> dragEndFunc = [](std::shared_ptr<NodeType> nd, std::shared_ptr<NodeType> nd2) { std::cout << "Drag " << nd->name << " to " << nd2->name << std::endl; };
		
		/*
			模板参数colDrawFuncs是 每个TreeNode右侧某列绘制的函数void(std::shared_ptr<NodeType>)，内部需要含 ImGui::NextColumn();
			example
			[](std::shared_ptr<NodeType> nd) {
				ImGui::NextColumn();
				ImGui::Text("ColText");
			}
	   */
		std::vector<std::function<void(std::shared_ptr<NodeType>)>> colDrawFuncs;

		void ShowWin()
		{
			if (show)
			{
				ImGui::Begin("Hierachy", &show);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

				Show();

				ImGui::End();
			}
		}

		void Show()
		{
			if (root != nullptr)
			{
				int colNum = colDrawFuncs.size();
				ImGui::Columns(colNum + 1, root->name.c_str(), true);
				InitHierachy_Rcs(root, root, 0, 0);
			}
		}

		//注意这里“Ui函数保存了nd引用”，当删除nd的时候需要重新赋值lambda清除引用
		void ClearUi()
		{
			dragNode = nullptr;
			root = nullptr;
			clickFunc = [](std::shared_ptr<NodeType> nd) {};
			rightClickUi = [](std::shared_ptr<NodeType> nd) {};
			dragEndFunc = [](std::shared_ptr<NodeType> nd, std::shared_ptr<NodeType> nd2) {std::cout << "Drag " << nd->name << " to " << nd2->name << std::endl; };
			colDrawFuncs.clear();
		}
	private:
		void InitHierachy_Rcs(std::shared_ptr<NodeType> root, std::shared_ptr<NodeType> nd, int nodeId,int option)
		{
			//设置当前节点并根据当前节点的点击情况获取后续节点的展开配置
			int nextOption = 0;
			bool node_open = SetOneNode(root, nd, nodeId++,option, nextOption);

			//绘制节点右侧每列的属性
			for (int i = 0; i < colDrawFuncs.size(); i++)
			{
				colDrawFuncs[i](nd);
			}

			ImGui::NextColumn();//切换到下一行
			if (node_open)
			{
				for (auto& childnode : nd->child)
				{
					//后续节点的展开配置 = 父节点配置 | 当前节点点击情况所决定的配置
					InitHierachy_Rcs(root, childnode, nodeId++,option | nextOption);
				}
				ImGui::TreePop();
			}
		}

		bool SetOneNode(std::shared_ptr<NodeType> root, std::shared_ptr<NodeType> nowNode, int nodeId,int option, int& nextOption)
		{
			//根据父节点的option来决定当前节点是否展开
			if (option & (int)NodeOption::Open)
				ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			else if (option & (int)NodeOption::Close)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (nowNode->is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)nodeId, node_flags, nowNode->name.c_str());


			ImGuiIO& io = ImGui::GetIO();

			//节点选取模块:ImGui::GetIO虽然可以获取按键，但在win32窗口使用之后经常会获取不了，因此这里采用inputeDetect进行按键检测
			if (io.KeyCtrl && ImGui::IsItemClicked(0))
			{
				nowNode->is_selected = !nowNode->is_selected;
			}
			else if (io.KeyAlt && ImGui::IsItemClicked(0))
			{
				//ALT+鼠标左键，后续子节点展开
				nextOption |= (int)NodeOption::Open;
				SelectNdAndAllChild(nowNode, true);
			}
			else if (io.KeyShift && ImGui::IsItemClicked(0))
			{
				//LSHIFT+鼠标左键，后续子节点关闭
				nextOption |= (int)NodeOption::Close;
				SelectNdAndLayerChild(nowNode, true);
			}
			else if (ImGui::IsItemClicked(0))
			{
				SelectNdAndAllChild(root, false);
				nowNode->is_selected = true;
			}

			clickFunc(nowNode);

			//鼠标右键展开对当前节点的具体操作窗口
			if (ImGui::BeginPopupContextItem())
			{
				ImGui::Text("NodeName: %s   Node:Id: %d", nowNode->name.c_str(), nodeId);
				rightClickUi(nowNode);
				ImGui::EndPopup();
			}

			//节点拖动模块:拖动节点A到节点B时调用的函数
			static float activeTime = 0;

			if (ImGui::IsItemClicked(0))
			{
				dragNode = nowNode;
			}
			else if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseReleased(0))
			{
				if (nowNode != dragNode && activeTime > 10)
				{
					dragEndFunc(dragNode, nowNode);
					dragNode = nullptr;
				}
				activeTime = 0;
			}
			else if (ImGui::IsItemActive())
			{
				ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f); // Draw a line between the button and the mouse cursor
				activeTime++;
			}


			return node_open;
		}

		void SelectAllChild(std::shared_ptr<NodeType> nd, bool is_selected)
		{
			for (auto& child : nd->child)
			{
				child->is_selected = is_selected;
				SelectAllChild(child, is_selected);
			}
		}

		void SelectNdAndAllChild(std::shared_ptr<NodeType> nd, bool is_selected)
		{
			nd->is_selected = is_selected;
			SelectAllChild(nd, is_selected);
		}

		void SelectLayerChild(std::shared_ptr<NodeType> nd, bool is_selected)
		{
			for (auto& child : nd->child)
			{
				child->is_selected = is_selected;
			}
		}

		void SelectNdAndLayerChild(std::shared_ptr<NodeType> nd, bool is_selected)
		{
			nd->is_selected = is_selected;;
			SelectLayerChild(nd, is_selected);
		}
	};
}