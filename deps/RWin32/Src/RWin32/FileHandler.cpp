#pragma once
#include "../Include/RWin32/FileHandler.h"
#include <RWin32\Win32Debug.h>
#ifdef WIN32
#include <shlobj.h>
#endif

namespace RWin32
{

    void GetFilePathAndName(std::string dirPath,std::string splitCh, std::vector<std::string>& vecFilePath, std::vector<std::string>& vecFileName)
    {
        //�ļ����  
        //long hFile = 0;  //win7    intptr_t hFile = 0;   //win10
        intptr_t hFile = 0;
        //�ļ���Ϣ  
        struct _finddata_t fileinfo;
        std::string p;
        if ((hFile = _findfirst(p.assign(dirPath).append(splitCh+"*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                //�����Ŀ¼,����֮  
                //�������,�����б�  
                if ((fileinfo.attrib & _A_SUBDIR))
                {
                    if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                        GetFilePathAndName(p.assign(dirPath).append(splitCh).append(fileinfo.name), splitCh, vecFilePath, vecFileName);
                }
                else
                {
                    vecFilePath.push_back(dirPath + splitCh + fileinfo.name);
                    vecFileName.push_back(fileinfo.name);
                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
    }

    std::vector<std::string> Dialog_OpenFile(char* filter, bool multi)
    {
        std::vector<std::string> res;

        OPENFILENAME ofn;
        TCHAR szOpenFileNames[80 * MAX_PATH];
        TCHAR szPath[MAX_PATH];
        TCHAR allPath[80 * MAX_PATH];
        TCHAR* p;
        int nLen = 0;

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szOpenFileNames;
        ofn.nMaxFile = sizeof(szOpenFileNames);
        ofn.lpstrFile[0] = '\0';
        ofn.lpstrFilter = filter;
        ofn.Flags = OFN_EXPLORER;

        if (multi == true)
        {
            ofn.Flags |= OFN_ALLOWMULTISELECT;

            if (GetOpenFileName(&ofn))
            {
                /*
                ���ļ��ĸ�·�����Ƶ�szPath,��:
                choosefile:    C:\Model\Model1.fbx C:\Model\Model2.fbx
                szPath:  C:\Model
                */
                lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
                lstrcat(szPath, TEXT("\\\0"));

                p = szOpenFileNames + ofn.nFileOffset; //��ָ���Ƶ���һ���ļ�

                ZeroMemory(allPath, sizeof(allPath));
                //�����ļ���
                while (*p)
                {
                    TCHAR mNowPath[MAX_PATH];
                    ZeroMemory(mNowPath, sizeof(mNowPath));
                    lstrcat(mNowPath, szPath);  //���ļ�������·��  
                    lstrcat(mNowPath, p);    //�����ļ���  
                    res.push_back(mNowPath);
                   
                    lstrcat(allPath, mNowPath);  
                    lstrcat(allPath, TEXT("\n"));
                    p += lstrlen(p) + 1;     //������һ���ļ�
                }
                MessageBox(NULL, allPath, TEXT("MultiSelect"), MB_OK | MB_TOPMOST);
            }
        }
        else
        { 
            ZeroMemory(szOpenFileNames, sizeof(szOpenFileNames));
            if (GetOpenFileName(&ofn))
            {
                MessageBox(NULL, szOpenFileNames, TEXT("SingleSelect"), MB_OK | MB_TOPMOST);
                res.push_back(szOpenFileNames);
            }
        }
        return res;
    }

	std::string Dialog_SaveFile(char* filter, char* defExt)
	{
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));

		TCHAR filePath[MAX_PATH] = { 0 };
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = defExt;
		ofn.lpstrTitle = "Save To";
		ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
		ofn.FlagsEx = OFN_EX_NOPLACESBAR;
		ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
		ofn.hwndOwner = NULL;
		if (GetSaveFileName(&ofn))
		{
			MessageBox(NULL, filePath, TEXT("Save"), MB_OK | MB_TOPMOST);
		}
		return filePath;
	}

	// ѡ��Ŀ¼
	void Dialog_SelectDir()
	{
		BROWSEINFO  bi;
		bi.hwndOwner = NULL;
		bi.pidlRoot = CSIDL_DESKTOP;//�ļ��еĸ�Ŀ¼���˴�Ϊ����
		bi.pszDisplayName = NULL;
		bi.lpszTitle = NULL;//��ʾλ�ڶԻ������ϲ�����ʾ��Ϣ
		bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;//���½��ļ��а�ť
		bi.lpfn = NULL;
		bi.iImage = 0;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);//����ѡ��Ի���
		if (pidl == NULL)
		{
			std::cout << "û��ѡ��Ŀ¼" << std::endl;
			return;
		}
		TCHAR strFolder[MAX_PATH];
		SHGetPathFromIDList(pidl, strFolder);
		std::string sFolder = strFolder;
	}

	void Bin_WriteStr(std::ofstream& outFile, std::string str)
	{
		uint32_t sz = static_cast<uint32_t>(str.size());
		outFile.write(reinterpret_cast<char*>(&sz), sizeof(sz));
		outFile.write(str.c_str(), sz);
	}

	std::string Bin_LoadStr(std::ifstream& inFile)
	{
		uint32_t sz;
		inFile.read(reinterpret_cast<char*>(&sz), sizeof(sz));

		std::string str;
		str.resize(sz);

		inFile.read(str.data(), sz);
		return str;
	}



	void Bin_WriteStrVec(std::ofstream& outFile, std::vector<std::string>& vec)
	{
		uint32_t eleNum = static_cast<uint32_t>(vec.size());
		outFile.write(reinterpret_cast<char*>(&eleNum), sizeof(eleNum));
		
		for (auto& str : vec)
		{
			Bin_WriteStr(outFile, str);
		}
	}

	std::vector<std::string> Bin_LoadStrVec(std::ifstream& inFile)
	{
		uint32_t eleNum;
		inFile.read(reinterpret_cast<char*>(&eleNum), sizeof(eleNum));

		std::vector<std::string> vec;
		for (int i = 0; i < eleNum; i++)
		{
			vec.emplace_back(Bin_LoadStr(inFile));
		}
		return vec;
	}

}