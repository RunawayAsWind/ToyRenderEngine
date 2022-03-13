#pragma once
//为什么要  #define WIN32_LEAN_AND_MEAN 
//在包含 WIN32/MFC SDK 头文件之前定义些宏, 可以通过预处理来关闭掉一些不太常用的系统接口或参数, 这样可以加快编译的速度
//不加载MFC所需的模块。在编译链接时，以及在最后生成的一些供调试用的模块时，速度更快，容量更小。
#define WIN32_LEANN_AND_MEAN

#include <windows.h>
#include <tchar.h>
//引入线程头文件，标准库
#include <process.h>
#include <utility>

namespace RWin32
{
    //将当前窗口移动到屏幕中心,并设置宽高
    void Win32_MoveWinToCenter(HWND hWnd, int nWidth, int nHeight, bool bRepaint = false);

    //获取窗口宽高
    std::pair<int, int> Win32_GetWidthAndHeight(HWND hWnd);

    //创建没有附加数据、风格默认，没有菜单的简单窗口类
    WNDCLASSEXW Win32_SimpleWinClass(HINSTANCE hInstance, WNDPROC lpfnWndProc, LPCWSTR lpszClassName);

    //创建无尺寸的窗口（尺寸可通过WJAPI_Win32_MoveWinToCenter设定）
    HWND Win32_CreWin_NoSize(HINSTANCE hInstance,LPCWSTR lpClassName, LPCWSTR lpWindowName,HWND hWndParent=NULL,DWORD dwExStyle= WS_EX_RIGHTSCROLLBAR, DWORD dwStyle= WS_OVERLAPPEDWINDOW,HMENU hMenu=NULL,LPVOID lpParam=NULL);

    //直接开始一个线程，传入的是线程函数和传给线程函数的值
    /*
    线程函数格式如下
    DWORD WINAPI ThreadFun(LPVOID pM)
    {
        WJAPI_Win32_DebugOut(TEXT("子线程的线程ID号为：%d  线程传值为：%d  \n"),GetCurrentThreadId(),(int)pM);
    }
    */
    HANDLE Win32_CreThread(_beginthreadex_proc_type _StartAddress, void* _ArgList);

}
