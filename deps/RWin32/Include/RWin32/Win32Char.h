#pragma once
#include "Win32Debug.h"
#include <tchar.h>
#include <regex>
#include <stdarg.h>

//用于wstring与string转换
#pragma comment(lib, "comsuppw.lib")

namespace RWin32
{
	//读取Unicode编码文件的所有内容，存入wstring中
	std::wstring Win32_GetUnicodeFileTxt(const wchar_t* lpFile);

	//wstring转string
	std::string Win32_Ws2Str(std::wstring wstr);

	//string转wstring
	std::wstring Win32_Str2Ws(std::string str);

	//基于正则表达式的wstring内容替换
	std::wstring Win32_WsReplace(std::wstring str, const wchar_t* before, const wchar_t* after);

	//基于正则查询的wstring内部数字查找（只查找匹配的第一个并返回）
	int Win32_WsFindNum(std::wstring str);

	//wstring(宽字符)转成常量字符,需要手动释放内存(delete [])
	char* Win32_Ws2Ch(std::wstring wstr);

	//将string转为wchar_t*,需要手动释放内存(delete [])
	wchar_t* Win32_Str2Wch(std::string pKey);
}

