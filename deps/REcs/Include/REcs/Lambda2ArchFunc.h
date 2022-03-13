#pragma once
#include <RTmpl/Hash.h>
#include "Archetype.h"
#include <functional>

namespace REcs {

	/*
	将lambda函数形式[](Cmpt1 a,Cmpt2 b,Cmpt3....){do update}
	变换成void(Archetype*, size_t mMeshIdx){[](GetCmptPtr<Cmpt1>,GetCmptPtr<Cmpt2>,GetCmptPtr<Cmpt3>...){do update}}的函数形式
	*/
	class ArchUniFunc
	{
	private:
		std::vector<std::string> cmptNameVec;				//记录该函数需要访问的组件，后续会寻找满足Vec中所有组件的Archetype
		std::function<void(Archetype*, size_t)> uniFunc;	//在对Cmpt操作的lambda外面又包装了一层函数，对某个Archetype的第size_t个实体数据进行lambda操作
	public:
		template<typename... Args>
		ArchUniFunc(std::function<void(Args*...)>&& srcFunc)
		{
			(cmptNameVec.push_back(RTmpl::Type_SimpleName<Args>()), ...);
			uniFunc = std::function
			(
				[srcFunc = std::forward<std::function<void(Args*...)>>(srcFunc)]
				(Archetype* archetype, size_t archGlobalIdx)
				{
					srcFunc(archetype->GetCmptPtr<Args>(archGlobalIdx)...);
				}
			);
		}
		std::function<void(Archetype*, size_t)> GetFunc() const noexcept { return uniFunc; }
		std::vector<std::string> GetCmptMetaVec() const noexcept{ return cmptNameVec; }
	};

	template<typename Func>
	auto Lambda2ArchFunc(Func&& lambda)
	{return ArchUniFunc(std::function(lambda));}
}