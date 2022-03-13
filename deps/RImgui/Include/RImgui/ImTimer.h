#pragma once
#include "../../3rdParty/Imgui/imgui.h"
#include <queue>

namespace RImgui
{
	class CurveDraw
	{
	private:
		float values[90] = {};
		int values_offset = 0;
		double refresh_time = 0.0;

		std::string name;
		float* updateData;
		float minVal;
		float maxVal;
	public:
		CurveDraw(std::string name, float& updateData, float minVal, float maxVal) :name(name), updateData(&updateData), minVal(minVal), maxVal(maxVal) {}

		void Update(bool animate)
		{
			if (!animate || refresh_time == 0.0)
			{
				refresh_time = ImGui::GetTime();
			}
			while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 Hz rate for the demo
			{
				values[values_offset] = *updateData;
				values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
				refresh_time += 1.0f / 60.0f;
			}
			{
				float average = 0.0f;
				for (int n = 0; n < IM_ARRAYSIZE(values); n++)
					average += values[n];
				average /= (float)IM_ARRAYSIZE(values);
				char overlay[32];
				sprintf(overlay, "avg %f", average);
				ImGui::PlotLines(name.c_str(), values, IM_ARRAYSIZE(values), values_offset, overlay, 0, 100.0f, ImVec2(0, 80.0f));
			}
		}
	};

	class ImTimer
	{
	public:
		bool show = false;
		bool animate = true;
		std::function<void()> updateUi_Timer = []() {ImGui::Text("Hello from ImTimer::updateUi_Timer!"); };
		std::vector<CurveDraw> vecCurve;

		void ShowWin()
		{
			if (show)
			{
				ImGui::Begin("Timer", &show);

				Show();

				ImGui::End();
			}
		}

		void Show()
		{
			ImGui::Checkbox("Animate", &animate);

			for (auto& curve : vecCurve)
			{
				curve.Update(animate);
			}
			ImGui::Spacing();
		}
	};
}