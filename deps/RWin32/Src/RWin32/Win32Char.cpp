#include   "../Include/RWin32/Win32Char.h"
#include <assert.h>
#include <comutil.h>

//Read Txt File to wstring
/*
1. ANSI code---no format mark.
file size is 0 when the file content is empty.

2. UTF-16-LE(UCS-2-LE) and UTF-16-BE(UCS-2-BE)-----2 bytes at the beginning are mark. "0xFF 0xFE"or"0xFE 0xFF"
file size is 2 when the file content is empty.Each char is 2 bytes

3. UTF-8-BOM-----3 bytes at the beginning are mark。0xEF, 0xBB, 0xBF。
file size is 3 when the file content is empty.Each char is 1~4 bytes

CodeFrom:https://bbs.csdn.net/topics/310113168?depth_1-utm_source=distribute.pc_relevant.none-task-discussion_topic-BlogCommendFromBaidu-1&utm_source=distribute.pc_relevant.none-task-discussion_topic-BlogCommendFromBaidu-1

*/

namespace RWin32
{
	std::wstring Win32_GetUnicodeFileTxt(const wchar_t* lpFile)
	{
		std::wstring wstr;

		PBYTE pBuffer, pConv, pText;	//PBYTE = BYTE*
		BYTE pSwap;
		DWORD dwByteRead=0;

		HANDLE handle = CreateFileW(lpFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		assert(handle != INVALID_HANDLE_VALUE);
		int iFileLength = GetFileSize(handle, NULL);

		/*
		|------------filedata---------------| |----------------end \0---------------|
		byte[0] byte[1] ....byte[ifilelength] byte[ifilelength+1] byte[ifilelength+2]
		*/
		pBuffer = (PBYTE)malloc(iFileLength + 2);
		pBuffer[iFileLength] = 0;
		pBuffer[iFileLength + 1] = 0;

		assert(ReadFile(handle, pBuffer, iFileLength, &dwByteRead, NULL));
		CloseHandle(handle);

		WIN32DEBUG(L"\n");
		WIN32DEBUG(L"---------------TxtReader--------------------\n");
		WIN32DEBUG(L"Win32Char__FileLength:  %d      ReadBytes: %d\n", iFileLength, dwByteRead);

		WIN32DEBUG(L"\n");
		for (int i = 0; i < min(iFileLength, 10); i++)
		{
			WIN32DEBUG(L"0x%02X\n", pBuffer[i]);
		}
		WIN32DEBUG(L"\n");

		int flags = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;

		/*
		|-----------IsTextUnicode?----------|
		|------------filedata---------------| |----------------end \0---------------|
		byte[0] byte[1] ....byte[ifilelength] byte[ifilelength+1] byte[ifilelength+2]
		*/

		//if (IsTextUnicode(pBuffer, iFileLength, &flags))
		if (iFileLength >= 2 && ((pBuffer[0] == 0xFF && pBuffer[1] == 0xFE) || (pBuffer[0] == 0xFE && pBuffer[1] == 0xFF)))
		{
			WIN32DEBUG(L"Check:  Is Unicode\n");

			/*
			|           2byte          |iFileLength - 2 (May Reverse)|
			|0xFF和0xFE"或者"0xFE和0xFF|----------txtdata-----------|
			|--------------------filedata---------------------------| |----------------end \0---------------|
				byte[0]        byte[1]  byte[2] ....byte[ifilelength] byte[ifilelength+1] byte[ifilelength+2]
										  /\
								  pText = pBuffer + 2;
									   |---------------------- pConv Length = iFileLength ------------------|
			*/
			pText = pBuffer + 2;
			int txtLength = iFileLength - 2;

			//if (flags & IS_TEXT_UNICODE_REVERSE_SIGNATURE)//UCS-2-BE, reverse the txtdata
			if ((pBuffer[0] == 0xFE && pBuffer[1] == 0xFF))
			{
				for (int i = 0; i < txtLength / 2; ++i)
				{
					pSwap = pText[2 * i];
					pText[2 * i] = pText[2 * i + 1];
					pText[2 * i + 1] = pSwap;
				}
			}

			WIN32DEBUG(L"\n");
			for (int i = 0; i < min(iFileLength, 10); i++)
			{
				WIN32DEBUG(L"0x%02X\n", pBuffer[i]);
			}
			WIN32DEBUG(L"\n");

			pConv = (PBYTE)malloc(iFileLength);

#ifdef UNICODE
			//File is Unicode，Compile Character set is Unicode，   wide char ---> wide char
			wcscpy_s((WCHAR*)pConv, iFileLength / (sizeof(wchar_t)), (WCHAR*)pText);

			wstr = std::wstring((wchar_t*)pConv);
#else
			//File is Unicode，Compile Character set is Not Unicode，   wide char ---> multi byte
			WideCharToMultiByte(CP_ACP, 0, (LPCWCH)pText, -1, (LPSTR)pConv, iFileLength, NULL, NULL);

			wstr = std::wstring(Win32_Str2Ws(std::string((char*)pConv)));
#endif

		}
		else
		{
			WIN32DEBUG(L"Check:  Not Unicode......\n");

			if (iFileLength >= 3 && pBuffer[0] == 0xEF && pBuffer[1] == 0xBB && pBuffer[2] == 0xBF)
			{
				WIN32DEBUG(L"Check:  UTF-8-BOM...... Can't Read\n");
				pConv = (PBYTE)malloc(iFileLength);
				wstr = std::wstring(L"Can't Read UTF-8-BOM");
			}
			else
			{
				WIN32DEBUG(L"Check:  Maybe UTF-8 or ANSI ,Try to read As ANSI\n");

				pText = pBuffer;
				/*
				| ----------filedata----------- | |----------------end \0-------------------|
					byte[0] ....byte[ifilelength] byte[ifilelength + 1] byte[ifilelength + 2]
					  /\
				 pText = pBuffer
				| --------------------- pConv Length = iFileLength + 2 ---------------------|
				*/

#ifdef UNICODE
				//File is Not Unicode，Compile Character set is  Unicode    char ---> wchar
				size_t strLen = iFileLength + 1;
				size_t wstrDataLen = 2 * (strLen);
				pConv = (PBYTE)malloc(wstrDataLen);
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pText, -1, (LPWSTR)pConv, strLen);

				wstr = std::wstring((wchar_t*)pConv);
#else
				//File is Not Unicode，Compile Character set is Not Unicode    char ---> char
				pConv = (PBYTE)malloc(static_cast<size_t>(iFileLength) + 2);
				strcpy_s((char*)pConv, iFileLength+2,(char*)pText);

				wstr = std::wstring(Win32_Str2Ws(std::string((char*)pConv)));
#endif
			}

		}

		free(pConv);
		free(pBuffer);

		WIN32DEBUG(L"--------------- TxtReader End ---------------\n");
		WIN32DEBUG(L"\n");

		return wstr;
	}

	//代码来源：https://blog.csdn.net/mercy_ps/article/details/81218608 
	std::string Win32_Ws2Str(std::wstring wstr)
	{
		_bstr_t t = wstr.c_str();
		char* pchar = (char*)t;
		std::string result = pchar;
		return result;
	}

	//代码来源：https://blog.csdn.net/mercy_ps/article/details/81218608 
	std::wstring Win32_Str2Ws(std::string str)
	{
		_bstr_t t = str.c_str();
		wchar_t* pwchar = (wchar_t*)t;
		std::wstring result = pwchar;
		return result;
	}

	std::wstring Win32_WsReplace(std::wstring str, const wchar_t* before, const wchar_t* after)
	{
		std::wregex wre(before);
		std::wstring newstr = std::regex_replace(str, wre, after);
		return newstr;
	}

	int Win32_WsFindNum(std::wstring str)
	{
		int result = 0;
		std::wregex wre(L"(\\d)");
		std::wsmatch wm;
		if (std::regex_search(str, wm, wre))
		{
			return std::stoi(wm.str(1));
		}
		return result;
	}

	//代码来源：https://stackoverflow.com/questions/4387288/convert-stdwstring-to-const-char-in-c
	char* Win32_Ws2Ch(std::wstring wstr)
	{
		const wchar_t* input = wstr.c_str();
		size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
		char* buffer = new char[size];

		size_t convertedSize;
		size_t ret = wcstombs_s(&convertedSize, buffer, size, input, size);

		return buffer;
	}

	//代码来源：https://blog.csdn.net/qq1841370452/article/details/81697313
	wchar_t* Win32_Str2Wch(std::string pKey)
	{
		int length = static_cast<int>(pKey.length() + 1);
		char* pCStrKey = const_cast<char*>(pKey.c_str());
		//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
		int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, length, NULL, 0);
		wchar_t* pWCStrKey = new wchar_t[pSize];
		//第二次调用将单字节字符串转换成双字节字符串
		MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, length, pWCStrKey, pSize);
		return pWCStrKey;
	}
}