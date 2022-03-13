#pragma once
#include "../../3rdParty/Imgui/imgui.h"
#include <string>
#include <functional>

namespace RImgui
{
	class ImWin
	{
	public:
		bool show = false;
		std::string name = "ImWin";
		std::function<void()> updateUi = []() {ImGui::Text("Hello from  ImWin::updateUi!"); };

		ImWin(std::string name) :name(name)
		{}

		void ShowWin()
		{
			if (show)
			{
				ImGui::Begin(name.c_str(), &show);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

				updateUi();

				ImGui::End();
			}
		}
	};
}