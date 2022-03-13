#pragma once
#include <RWIN32/Win32Debug.h>
#include <thread>

namespace RDx12
{
    template<typename FrameWork>
    class RenderThread
    {
    private:
        bool loadFinish = false;
        std::thread ResLoadTh;
        std::thread RenderTh;

        void ResLoadFunc()
        {
            WIN32DEBUG(L"____________Thread_ResLoad(Start)____________\n");
            fw.InitRes();
            loadFinish = true;
            WIN32DEBUG(L"____________Thread_ResLoad(End)____________\n");
        }

        void RenderFunc()
        {
            WIN32DEBUG(L"____________Thread_Render(Start)____________\n");
            while (!loadFinish)
            {
            }
            fw.StartRender();
            WIN32DEBUG(L"____________Thread_Render(End)____________\n");
        }

    public:
        FrameWork fw;
        RenderThread(HINSTANCE hInst, HWND hWnd) :fw{ hInst,hWnd }
        {
            ResLoadTh = std::thread(&RenderThread<FrameWork>::ResLoadFunc, this);
            RenderTh = std::thread(&RenderThread<FrameWork>::RenderFunc, this);
        }

        void Load() { ResLoadTh.detach(); }
        void Start() { RenderTh.detach(); }
        void Resume() { fw.ResumRender(); }
        void Stop() { fw.StopRender(); }
        void Quit() { fw.QuitRender(); }
    };
}