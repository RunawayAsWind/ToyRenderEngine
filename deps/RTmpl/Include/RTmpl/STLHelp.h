#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>    
#include <memory>
#include <sstream> 

namespace RTmpl
{

    template<typename Vec>
    void ShowVecInfo(Vec vec)
    {
        for (const auto& data : vec)
        {
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }

    template<typename Vec>
    void Show2DVecInfo(Vec vec)
    {
        for (const auto& line : vec)
        {
            for (const auto& data : line)
            {
                std::cout << data << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    template<typename MapType>
    void ShowPairInfo(MapType map)
    {
        for (const auto& [k, v] : map)
        {
            std::cout << std::dec << k << " : " << v << "  ";
        }
        std::cout << std::endl;
    }

    template<typename KeyType, typename ValType>
    std::vector<std::pair<KeyType, ValType>> SortUnOrdMap(std::unordered_map<KeyType, ValType> map)
    {
        std::vector<std::pair<KeyType, ValType>> tmp;
        for (auto& i : map)
            tmp.push_back(i);

        std::sort(tmp.begin(), tmp.end(),
            [=](std::pair<KeyType, ValType>& a, std::pair<KeyType, ValType>& b) { return a.second < b.second; });
        return tmp;
    }

    template<typename StrType>
    StrType StrReplaceAll(StrType str, const StrType& from, const StrType& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != StrType::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }

	template<typename StrType>
	StrType ToLeftPath(StrType str)
	{
		return RTmpl::StrReplaceAll(str, std::string("/"), std::string("\\"));
	}

	template<typename StrType>
	StrType ToRightPath(StrType str)
	{
		return RTmpl::StrReplaceAll(str, std::string("\\"), std::string("/"));
	}

    template<typename StrType>
    std::vector<StrType> StrSplit(const StrType& str, const  StrType& pattern)
    {
        std::vector< StrType> res;
        if (str.size()<1)
            return res;
        //在字符串末尾也加入分隔符，方便截取最后一段
        StrType strs = str + pattern;
        size_t pos = strs.find(pattern);

        while (pos != strs.npos)
        {
            StrType temp = strs.substr(0, pos);
            if(temp.size()>=1)
            res.push_back(temp);
            //去掉已分割的字符串,在剩下的字符串中进行分割
            strs = strs.substr(pos + 1, strs.size());
            pos = strs.find(pattern);
        }

        return res;
    }

    //用于检查weak_ptr是否为空
    template <typename T>
    bool If_Weak_UnInit(std::weak_ptr<T> const& weak) {
        using wt = std::weak_ptr<T>;
        return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
    }

    template<typename T>
    bool InVector(T data, std::vector<T> vec)
    {
        bool find = false;
        for (auto& vecdata : vec)
        {
            if (data == vecdata)
            {
                return true;
            }
        }
        return false;
    }

	template <class Type>
	Type StringToNum(const std::string& str)
	{
		std::istringstream iss(str);
		Type num;
		iss >> num;
		return num;
	}

    std::string GetFileName(std::string path);
	std::string GetFileNameWithOutEx(std::string path);
    std::wstring GetFileName(std::wstring path);
}