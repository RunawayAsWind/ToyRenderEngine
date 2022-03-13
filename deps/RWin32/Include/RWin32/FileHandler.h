#pragma once
#include <io.h>
#include <vector>
#include<windows.h>
#include <iostream>
#include <fstream>

namespace RWin32
{

	/*
		递归获取文件夹下的文件

		std::string dirPath 文件夹路径
		std::string splitCh 路径分隔符，"/"或"\\"
		std::vector<std::string>& vecFilePath 存储文件的路径及名称(eg.   C:\Users\WUQP\Desktop\test_devided\data1.txt)
		std::vector<std::string>& vecFileName  只存储文件的名称(eg.     data1.txt)
	*/
	void GetFilePathAndName(std::string dirPath, std::string splitCh, std::vector<std::string>& vecFilePath, std::vector<std::string>& vecFileName);
 
    std::vector<std::string> Dialog_OpenFile(char* filter = "All Files(*.*)\0*.*\0", bool multi = true);//进行文件选取,选取对象路径存入vector
	std::string Dialog_SaveFile(char* filter = "Data Files(*.data)\0*.data\0", char* defExt="data");
	void Dialog_SelectDir();

	void Bin_WriteStr(std::ofstream& outFile, std::string str);
	std::string Bin_LoadStr(std::ifstream& inFile);
	void Bin_WriteStrVec(std::ofstream& outFile, std::vector<std::string>& vec);
	std::vector<std::string> Bin_LoadStrVec(std::ifstream& inFile);

	template<typename T>
	void Bin_WriteData(std::ofstream& outFile, T data)
	{
		outFile.write(reinterpret_cast<char*>(&data), sizeof(data));
	}

	template<typename T>
	T Bin_LoadData(std::ifstream& inFile)
	{
		T data;
		inFile.read(reinterpret_cast<char*>(&data), sizeof(data));
		return data;
	}

	template<typename T>
	void Bin_WriteVec(std::ofstream& outFile, std::vector<T>& vec)
	{
		uint32_t sz = static_cast<uint32_t>(vec.size() * sizeof(T));
		outFile.write(reinterpret_cast<char*>(&sz), sizeof(sz));
		outFile.write(reinterpret_cast<char*>(vec.data()), sz);
	}

	template<typename T>
	std::vector<T> Bin_LoadVec(std::ifstream& inFile)
	{
		uint32_t sz;
		inFile.read(reinterpret_cast<char*>(&sz), sizeof(sz));

		std::vector<T> vec;
		vec.resize(sz / sizeof(T));

		inFile.read(reinterpret_cast<char*>(vec.data()), sz);

		return vec;
	}

	template<typename T>
	void Bin_WriteSerializeVec(std::ofstream& outFile, std::vector<T>& vec)
	{
		uint32_t eleNum = static_cast<uint32_t>(vec.size());
		outFile.write(reinterpret_cast<char*>(&eleNum), sizeof(eleNum));
		for (auto& data : vec)
		{
			data.RSerialize_Save(outFile);
		}
	}

	template<typename T>
	std::vector<T> Bin_LoadSerializeVec(std::ifstream& inFile)
	{
		uint32_t eleNum;
		inFile.read(reinterpret_cast<char*>(&eleNum), sizeof(eleNum));

		std::vector<T> vec;
		for (int i = 0; i < eleNum; i++)
		{
			T data;
			data.RSerialize_Load(inFile);
			vec.emplace_back(data);
		}
		return vec;
	}

}