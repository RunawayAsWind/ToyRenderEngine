#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <tchar.h>

//变参宏__VA_ARGS__，用于实现变参函数嵌套 https://blog.csdn.net/wangkai_123456/article/details/75008576
#define WIN32OUT(fm, ...) std::cout<<(RWin32::Win32_GetFormCh(fm,__VA_ARGS__).c_str())
#define WIN32DEBUG(fm, ...) OutputDebugStringW(RWin32::Win32_GetFormWCh(fm,__VA_ARGS__).c_str())
#define WIN32MSG(fm, ...)  MessageBoxW(NULL, RWin32::Win32_GetFormWCh(fm,__VA_ARGS__).c_str(), L"提示", MB_OK);

namespace RWin32
{
    //代码来自：https://blog.csdn.net/chenlycly/article/details/38342609
    inline std::string Win32_GetFormCh(const char* szFormat, ...)
    {
        std::string strResult = "";
        if (szFormat != NULL)
        {
            va_list pArgList;
            va_start(pArgList, szFormat);

            size_t nLength = _vscprintf(szFormat, pArgList) + 1;		//获取格式化字符串长度 _vscwprintf：获取格式化的宽字符，并返回长度（不包括结束符）
            std::vector<char> vBuffer(nLength, L'\0');			//创建用于存储格式化字符串的字符数组
            size_t nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, szFormat, pArgList);
            if (nWritten > 0)
            {
                strResult = &vBuffer[0];
            }

            va_end(pArgList);
        }
        return strResult;
    }

    //代码来自：https://blog.csdn.net/chenlycly/article/details/38342609
    inline std::wstring Win32_GetFormWCh(const wchar_t* szFormat, ...)
    {
        std::wstring strResult = L"";
        if (szFormat != NULL)
        {
            va_list pArgList;
            va_start(pArgList, szFormat);

            size_t nLength = _vscwprintf(szFormat, pArgList) + 1;		//获取格式化字符串长度 _vscwprintf：获取格式化的宽字符，并返回长度（不包括结束符）
            std::vector<wchar_t> vBuffer(nLength, L'\0');			//创建用于存储格式化字符串的字符数组
            size_t nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, szFormat, pArgList);
            if (nWritten > 0)
            {
                strResult = &vBuffer[0];
            }

            va_end(pArgList);
        }
        return strResult;
    }

    inline std::string HrToString(HRESULT hr)
    {
        char s_str[64] = {};
        sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
        return std::string(s_str);
    }

    class HrException : public std::runtime_error
    {
    public:
        HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
        HRESULT Error() const { return m_hr; }
    private:
        const HRESULT m_hr;
    };

    inline HRESULT ThrowIfFailed(HRESULT hr, const wchar_t* suc = NULL, const wchar_t* fail = NULL)
    {
        if (FAILED(hr))
        {
            WIN32DEBUG(fail);
            throw HrException(hr);
		}
        else
        {
            WIN32DEBUG(suc);
        }
        return hr;
    }
}