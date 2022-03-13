#include "../Include/RWin32/Win32Ui.h"
#include "../Include/RWin32/Win32Debug.h"

namespace RWin32
{
	void Win32_MoveWinToCenter(HWND hWnd, int nWidth, int nHeight, bool bRepaint)
	{
		int nGoalmapmode = MM_TEXT;

		HDC hdc = GetDC(hWnd);
		int nRealmapmode = GetMapMode(hdc);
		ReleaseDC(hWnd, hdc);

		if (nRealmapmode == nGoalmapmode)//确定窗口的“映射模式”，在MM_TEXT的默认模式下窗口原点在左上角，且，y轴朝下,x轴朝右
		{
			RECT windowrect;
			windowrect.left = 0;
			windowrect.top = 0;
			windowrect.right = nWidth;
			windowrect.bottom = nHeight;
			AdjustWindowRect(&windowrect, WS_POPUP | WS_VISIBLE, FALSE);

			int nLefttopx = GetSystemMetrics(SM_CXSCREEN) / 2 - windowrect.right / 2;		//窗口左上角的横坐标
			int nLefttopy = GetSystemMetrics(SM_CYSCREEN) / 2 - windowrect.bottom / 2;		//窗口左上角的纵坐标

			MoveWindow(hWnd, nLefttopx, nLefttopy, nWidth, nHeight, bRepaint);
		}
	}

	std::pair<int, int> Win32_GetWidthAndHeight(HWND hWnd)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		int width = rc.right - rc.left;   //窗口的宽度
		int height = rc.bottom - rc.top;  //窗口的高度
		std::pair<int, int> wh(width, height);
		return wh;
	}

	WNDCLASSEXW Win32_SimpleWinClass(HINSTANCE hInstance, WNDPROC lpfnWndProc, LPCWSTR lpszClassName)
	{
		//https://www.cnblogs.com/visions/p/5582687.html
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);														//结构体大小
		wcex.style = CS_GLOBALCLASS;															//窗口类的风格https://www.cnblogs.com/zuibunan/p/4706293.html    https://docs.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
		wcex.lpfnWndProc = lpfnWndProc;												//窗口过程函数
		wcex.cbClsExtra = 0;														//窗口类的附加数据buffer的大小
		wcex.cbWndExtra = 0;														//窗口的附加数据buffer的大小
		wcex.hInstance = hInstance;													//当前模块的实例句柄
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);								//窗口图标句柄（默认）
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);									//鼠标的句柄（默认）
		wcex.hbrBackground = NULL;													//指定空画刷，阻止GDI对窗口进行渲染，提升性能
		wcex.lpszMenuName = NULL;													//窗口菜单的资源ID字符串（不需要菜单，为NULL）
		wcex.lpszClassName = lpszClassName;											//指定窗口类名
		wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);						//窗口的小图标句柄(默认)
		return wcex;
	}

	HWND Win32_CreWin_NoSize(HINSTANCE hInstance, LPCWSTR lpClassName, LPCWSTR lpWindowName, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LPVOID lpParam)
	{
		//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
		HWND hwnd = CreateWindowExW(
			dwExStyle,
			lpClassName,
			lpWindowName,
			dwStyle,
			0,					//窗口X
			0,					//窗口Y
			0,					//窗口width
			0,					//窗口height
			hWndParent,			//父窗口句柄
			hMenu,				//菜单句柄
			hInstance,
			lpParam				//自定义参数
		);

		return hwnd;
	}

	HANDLE Win32_CreThread(_beginthreadex_proc_type _StartAddress, void* _ArgList)
	{
		HANDLE thhl = (HANDLE)_beginthreadex
		(
			NULL,									//安全属性，一般为默认
			0,										//默认线程堆栈大小
			_StartAddress,							//线程调用函数
			_ArgList,								//传入渲染的窗口句柄
			0,										//创建之后立刻调度
			NULL									//不需要返回线程ID号
		);
		return thhl;
	}

}