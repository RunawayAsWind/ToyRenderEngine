#pragma once
#include "Lambda2ArchFunc.h"
#include <set>

namespace REcs {

	class SysManager
	{
		public:
			template<typename... Args>
			bool Register(std::string sysName, std::function<void(Args*...)>&& func)
			{return Register(sysName, ArchUniFunc(std::forward<std::function<void(Args*...)>>(func)));}
			bool Register(std::string sysName, ArchUniFunc func);
			bool SetOrder(std::string sysName1, std::string sysName2);
			void ShowInfo() const noexcept;
			std::unordered_map<std::string, ArchUniFunc> GetFuncMap() const noexcept { return sysFuncMap; };
			std::set<std::pair<std::string, std::string>> GetFuncOrder() const noexcept { return sysFuncOrder; };
		private:
			std::unordered_map<std::string, ArchUniFunc> sysFuncMap;
			std::set<std::pair<std::string, std::string>> sysFuncOrder;
	};
}