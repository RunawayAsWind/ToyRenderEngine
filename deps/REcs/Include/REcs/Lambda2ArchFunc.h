#pragma once
#include <RTmpl/Hash.h>
#include "Archetype.h"
#include <functional>

namespace REcs {

	/*
	��lambda������ʽ[](Cmpt1 a,Cmpt2 b,Cmpt3....){do update}
	�任��void(Archetype*, size_t mMeshIdx){[](GetCmptPtr<Cmpt1>,GetCmptPtr<Cmpt2>,GetCmptPtr<Cmpt3>...){do update}}�ĺ�����ʽ
	*/
	class ArchUniFunc
	{
	private:
		std::vector<std::string> cmptNameVec;				//��¼�ú�����Ҫ���ʵ������������Ѱ������Vec�����������Archetype
		std::function<void(Archetype*, size_t)> uniFunc;	//�ڶ�Cmpt������lambda�����ְ�װ��һ�㺯������ĳ��Archetype�ĵ�size_t��ʵ�����ݽ���lambda����
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