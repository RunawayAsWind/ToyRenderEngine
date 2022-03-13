#include "../Include/REcs/CmptsRefMap.h"
#include <cassert>

namespace REcs
{
	size_t CmptsRefMap::ArchetypeTotalSize() const
	{
		size_t sumSize = 0;
		for (const auto& [key, meta] : cmptDic)
		{
			sumSize += meta.size;
		}
		return sumSize;
	}

	size_t CmptsRefMap::Sizeof(std::string cmptKey) const
	{
		assert(cmptDic.find(cmptKey) != cmptDic.end());
		return cmptDic.find(cmptKey)->second.size;
	}

	size_t CmptsRefMap::Alignof(std::string cmptKey) const
	{
		assert(cmptDic.find(cmptKey) != cmptDic.end());
		return cmptDic.find(cmptKey)->second.align;
	}

	void CmptsRefMap::CopyConstruct(std::string cmptKey, void* dst, void* src) const
	{
		auto target = cmptDic.find(cmptKey);
		if (target != cmptDic.end() && target->second.copyConstr!=nullptr)
			target->second.copyConstr(dst, src);
		else
			memcpy(dst, src, Sizeof(cmptKey));
	}

	void CmptsRefMap::MoveConstruct(std::string cmptKey, void* dst, void* src) const
	{
		auto target = cmptDic.find(cmptKey);
		if (target != cmptDic.end() && target->second.moveConstr!=nullptr)
			target->second.moveConstr(dst, src);
		else
			memcpy(dst, src, Sizeof(cmptKey));
	}

	void CmptsRefMap::Destruct(std::string cmptKey, void* cmpt) const
	{
		auto target = cmptDic.find(cmptKey);
		if (target != cmptDic.end() && target->second.destr!=nullptr)
			target->second.destr(cmpt);
	}
}