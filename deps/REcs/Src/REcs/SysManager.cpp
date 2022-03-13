#include "../Include/REcs/SysManager.h"

namespace REcs 
{
	bool SysManager::Register(std::string sysName, ArchUniFunc func)
	{
		if (sysFuncMap.find(sysName) != sysFuncMap.end())
		{
			return false;
		}
		sysFuncMap.emplace(sysName, func);
		return true;
	}

	bool SysManager::SetOrder(std::string sysName1, std::string sysName2)
	{
		if (sysFuncMap.find(sysName1) == sysFuncMap.end())
		{
			return false;
		}
		if (sysFuncMap.find(sysName2) == sysFuncMap.end())
		{
			return false;
		}
		sysFuncOrder.insert(std::pair(sysName1, sysName2));
		return true;
	}

	void SysManager::ShowInfo() const noexcept
	{
		std::cout << std::endl;
		std::cout << "SystemManager ShowInfo():" << std::endl;

		std::cout << "sysFuncMap:" << std::endl;
		for (auto [k, v] : sysFuncMap)
		{
			std::cout << k << " ";
		}
		std::cout << std::endl;

		std::cout << "sysFuncOrder:" << std::endl;
		for (auto [k, v] : sysFuncOrder)
		{
			std::cout << k << "->" << v << " ";
		}
		std::cout << std::endl;
	}

}