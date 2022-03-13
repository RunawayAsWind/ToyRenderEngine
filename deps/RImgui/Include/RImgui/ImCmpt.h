#pragma once
#include "../../3rdParty/Imgui/imgui.h"
#include "ImUtil.h"
#include <map>

namespace RImgui
{
    class ImCmpt
    {
    public:
        bool show = false;
        std::string lastChooseCmptId;
        std::map<std::string, std::function<void()>> hasNodeMap;

        int lineNum = 6;
        std::map<std::string, std::function<void()>> chooseNodeMap;


        void ShowWin()
        {
            if (show)
            {
                ImGui::Begin("Cmpt", &show);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

                Show();

                ImGui::End();
            }
        }

        void Show()
        {
            for (auto const& [btnName, btnFunc] : hasNodeMap)
            {
                if (lastChooseCmptId == btnName)
                {
                    hasNodeMap[btnName]();
                }
            }

            ImGui::Spacing();

            int id = 0;
            for (auto const& [btnName, btnFunc] : hasNodeMap)
            {
                if (id != 0)
                    ImGui::SameLine();

                //如果是之前点击过的组件属性面板，用ActiveButton颜色显示，表示选中
                if (lastChooseCmptId == btnName)
                {
                    ImUtil::SetActiveButton(btnName, id, btnFunc);
                }
                else//否则正常颜色显示
                {
                    ImUtil::SetButton(btnName, id, btnFunc);
                }
                id++;
            }
            ImGui::Spacing();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
            ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
            ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

            id = 0;
            for (auto const& [btnName, btnFunc] : chooseNodeMap)
            {
                if (id != 0 && id % lineNum != 0)
                    ImGui::SameLine();
                ImUtil::SetButton(btnName, id, btnFunc);
                id++;
            }

            float scroll_x = ImGui::GetScrollX();
            float scroll_max_x = ImGui::GetScrollMaxX();
            ImGui::EndChild();
            ImGui::PopStyleVar(2);
            float scroll_x_delta = 0.0f;
            ImGui::SmallButton("<<");
            if (ImGui::IsItemActive())
                scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f;
            ImGui::SameLine();
            ImGui::Text("Scroll from code"); ImGui::SameLine();
            ImGui::SmallButton(">>");
            if (ImGui::IsItemActive())
                scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f;
            ImGui::SameLine();
            ImGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
            if (scroll_x_delta != 0.0f)
            {
                // Demonstrate a trick: you can use Begin to set yourself in the context of another window
                // (here we are already out of your child window)
                ImGui::BeginChild("scrolling");
                ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
                ImGui::EndChild();
            }
            ImGui::Spacing();
        }

        void ClearUi()
        {
            hasNodeMap.clear();
            chooseNodeMap.clear();
        }
    };
}