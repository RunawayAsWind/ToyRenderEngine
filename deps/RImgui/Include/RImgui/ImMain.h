#pragma once
#include <functional>
#include <d3d12.h>
#include <wrl.h>
#include "../../3rdParty/Imgui/imgui.h"
#include "../../3rdParty/Imgui/imgui_impl_dx12.h"
#include "../../3rdParty/Imgui/imgui_impl_win32.h"

namespace RImgui
{
    class ImMain
    {
    public:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDesHeap;
        bool show = true;
        std::function<void()> showMainWinUi = []() {ImGui::Text("Hello from ImMain::showMainWinUi!"); };
        std::function<void()> showOtherWin = []() {ImGui::Begin("Hello from ImMain::showOtherWin"); ImGui::End(); };

        void Create(HWND hwnd, ID3D12Device4* dev, int frameNum, DXGI_FORMAT fmt)
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(srvDesHeap.GetAddressOf()));

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            //<<Imgui输入中文用以下代码支持>>
            auto fonts = ImGui::GetIO().Fonts;
            fonts->AddFontFromFileTTF(
                "c:/windows/fonts/simhei.ttf",
                13.0f,
                NULL,
                fonts->GetGlyphRangesChineseSimplifiedCommon()
            );

            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();

            // Setup Platform/Renderer bindings
            ImGui_ImplWin32_Init(hwnd);
            ImGui_ImplDX12_Init(dev, frameNum,
                fmt, srvDesHeap.Get(),
                srvDesHeap->GetCPUDescriptorHandleForHeapStart(),
                srvDesHeap->GetGPUDescriptorHandleForHeapStart());
        }

        void Resize()
        {
            ImGui_ImplDX12_InvalidateDeviceObjects();
            ImGui_ImplDX12_CreateDeviceObjects();
        }

        void Draw(ID3D12GraphicsCommandList* cl)
        {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ShowMainWin();
            showOtherWin();

            cl->SetDescriptorHeaps(1, srvDesHeap.GetAddressOf());
            ImGui::Render();
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cl);
        }

        void Release()
        {
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        void ShowMainWin()
        {
            IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
            // Demonstrate the various window flags. Typically you would just use the default!
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = false;
            static bool no_move = false;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = false;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;

            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
            if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
            if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (no_close)           show = NULL; // Don't pass our bool* to Begin

            // Main body of the Demo window starts here.
            if (!ImGui::Begin("Dear ImGui Demo", &show, window_flags))
            {
                // Early out if the window is collapsed, as an optimization.
                ImGui::End();
                return;
            }

            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

            showMainWinUi();

            ImGui::End();
        }
    };
}