#pragma once
#include "../../3rdParty/Imgui/imgui.h"
#include <functional>

namespace RImgui
{
	class ImUtil
	{
	public:
		static void SetButton(std::string btnName, int i, std::function<void()> clickFunc)
		{
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button(btnName.c_str(), ImVec2(0, 30)))
			{
				clickFunc();
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}

		static void SetActiveButton(std::string btnName, int i, std::function<void()> clickFunc)
		{
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6, 0.6f, 0.35f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.57, 0.85f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.57, 0.85f, 0.8f));
			if (ImGui::Button(btnName.c_str(), ImVec2(0, 30)))
			{
				clickFunc();
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}
	};
}

