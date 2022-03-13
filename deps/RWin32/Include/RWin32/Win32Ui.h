#pragma once
//ΪʲôҪ  #define WIN32_LEAN_AND_MEAN 
//�ڰ��� WIN32/MFC SDK ͷ�ļ�֮ǰ����Щ��, ����ͨ��Ԥ�������رյ�һЩ��̫���õ�ϵͳ�ӿڻ����, �������Լӿ������ٶ�
//������MFC�����ģ�顣�ڱ�������ʱ���Լ���������ɵ�һЩ�������õ�ģ��ʱ���ٶȸ��죬������С��
#define WIN32_LEANN_AND_MEAN

#include <windows.h>
#include <tchar.h>
//�����߳�ͷ�ļ�����׼��
#include <process.h>
#include <utility>

namespace RWin32
{
    //����ǰ�����ƶ�����Ļ����,�����ÿ��
    void Win32_MoveWinToCenter(HWND hWnd, int nWidth, int nHeight, bool bRepaint = false);

    //��ȡ���ڿ��
    std::pair<int, int> Win32_GetWidthAndHeight(HWND hWnd);

    //����û�и������ݡ����Ĭ�ϣ�û�в˵��ļ򵥴�����
    WNDCLASSEXW Win32_SimpleWinClass(HINSTANCE hInstance, WNDPROC lpfnWndProc, LPCWSTR lpszClassName);

    //�����޳ߴ�Ĵ��ڣ��ߴ��ͨ��WJAPI_Win32_MoveWinToCenter�趨��
    HWND Win32_CreWin_NoSize(HINSTANCE hInstance,LPCWSTR lpClassName, LPCWSTR lpWindowName,HWND hWndParent=NULL,DWORD dwExStyle= WS_EX_RIGHTSCROLLBAR, DWORD dwStyle= WS_OVERLAPPEDWINDOW,HMENU hMenu=NULL,LPVOID lpParam=NULL);

    //ֱ�ӿ�ʼһ���̣߳���������̺߳����ʹ����̺߳�����ֵ
    /*
    �̺߳�����ʽ����
    DWORD WINAPI ThreadFun(LPVOID pM)
    {
        WJAPI_Win32_DebugOut(TEXT("���̵߳��߳�ID��Ϊ��%d  �̴߳�ֵΪ��%d  \n"),GetCurrentThreadId(),(int)pM);
    }
    */
    HANDLE Win32_CreThread(_beginthreadex_proc_type _StartAddress, void* _ArgList);

}
