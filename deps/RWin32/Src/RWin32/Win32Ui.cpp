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

		if (nRealmapmode == nGoalmapmode)//ȷ�����ڵġ�ӳ��ģʽ������MM_TEXT��Ĭ��ģʽ�´���ԭ�������Ͻǣ��ң�y�ᳯ��,x�ᳯ��
		{
			RECT windowrect;
			windowrect.left = 0;
			windowrect.top = 0;
			windowrect.right = nWidth;
			windowrect.bottom = nHeight;
			AdjustWindowRect(&windowrect, WS_POPUP | WS_VISIBLE, FALSE);

			int nLefttopx = GetSystemMetrics(SM_CXSCREEN) / 2 - windowrect.right / 2;		//�������Ͻǵĺ�����
			int nLefttopy = GetSystemMetrics(SM_CYSCREEN) / 2 - windowrect.bottom / 2;		//�������Ͻǵ�������

			MoveWindow(hWnd, nLefttopx, nLefttopy, nWidth, nHeight, bRepaint);
		}
	}

	std::pair<int, int> Win32_GetWidthAndHeight(HWND hWnd)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		int width = rc.right - rc.left;   //���ڵĿ��
		int height = rc.bottom - rc.top;  //���ڵĸ߶�
		std::pair<int, int> wh(width, height);
		return wh;
	}

	WNDCLASSEXW Win32_SimpleWinClass(HINSTANCE hInstance, WNDPROC lpfnWndProc, LPCWSTR lpszClassName)
	{
		//https://www.cnblogs.com/visions/p/5582687.html
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);														//�ṹ���С
		wcex.style = CS_GLOBALCLASS;															//������ķ��https://www.cnblogs.com/zuibunan/p/4706293.html    https://docs.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
		wcex.lpfnWndProc = lpfnWndProc;												//���ڹ��̺���
		wcex.cbClsExtra = 0;														//������ĸ�������buffer�Ĵ�С
		wcex.cbWndExtra = 0;														//���ڵĸ�������buffer�Ĵ�С
		wcex.hInstance = hInstance;													//��ǰģ���ʵ�����
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);								//����ͼ������Ĭ�ϣ�
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);									//���ľ����Ĭ�ϣ�
		wcex.hbrBackground = NULL;													//ָ���ջ�ˢ����ֹGDI�Դ��ڽ�����Ⱦ����������
		wcex.lpszMenuName = NULL;													//���ڲ˵�����ԴID�ַ���������Ҫ�˵���ΪNULL��
		wcex.lpszClassName = lpszClassName;											//ָ����������
		wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);						//���ڵ�Сͼ����(Ĭ��)
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
			0,					//����X
			0,					//����Y
			0,					//����width
			0,					//����height
			hWndParent,			//�����ھ��
			hMenu,				//�˵����
			hInstance,
			lpParam				//�Զ������
		);

		return hwnd;
	}

	HANDLE Win32_CreThread(_beginthreadex_proc_type _StartAddress, void* _ArgList)
	{
		HANDLE thhl = (HANDLE)_beginthreadex
		(
			NULL,									//��ȫ���ԣ�һ��ΪĬ��
			0,										//Ĭ���̶߳�ջ��С
			_StartAddress,							//�̵߳��ú���
			_ArgList,								//������Ⱦ�Ĵ��ھ��
			0,										//����֮�����̵���
			NULL									//����Ҫ�����߳�ID��
		);
		return thhl;
	}

}