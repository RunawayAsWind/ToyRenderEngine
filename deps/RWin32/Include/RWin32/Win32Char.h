#pragma once
#include "Win32Debug.h"
#include <tchar.h>
#include <regex>
#include <stdarg.h>

//����wstring��stringת��
#pragma comment(lib, "comsuppw.lib")

namespace RWin32
{
	//��ȡUnicode�����ļ����������ݣ�����wstring��
	std::wstring Win32_GetUnicodeFileTxt(const wchar_t* lpFile);

	//wstringתstring
	std::string Win32_Ws2Str(std::wstring wstr);

	//stringתwstring
	std::wstring Win32_Str2Ws(std::string str);

	//����������ʽ��wstring�����滻
	std::wstring Win32_WsReplace(std::wstring str, const wchar_t* before, const wchar_t* after);

	//���������ѯ��wstring�ڲ����ֲ��ң�ֻ����ƥ��ĵ�һ�������أ�
	int Win32_WsFindNum(std::wstring str);

	//wstring(���ַ�)ת�ɳ����ַ�,��Ҫ�ֶ��ͷ��ڴ�(delete [])
	char* Win32_Ws2Ch(std::wstring wstr);

	//��stringתΪwchar_t*,��Ҫ�ֶ��ͷ��ڴ�(delete [])
	wchar_t* Win32_Str2Wch(std::string pKey);
}

