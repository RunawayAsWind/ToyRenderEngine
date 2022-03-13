#pragma once
#include <unordered_map>
#include "CmptMeta.h"

namespace REcs
{
	class CmptsRefMap
	{
	public:
		size_t ArchetypeTotalSize() const;
		size_t Sizeof(std::string cmptKey) const;
		size_t Alignof(std::string cmptKey) const;
		void CopyConstruct(std::string cmptKey, void* dst, void* src) const;
		void MoveConstruct(std::string cmptKey, void* dst, void* src) const;
		void Destruct(std::string cmptKey, void* cmpt) const;

		void Register(CmptMeta meta)
		{
			cmptDic[meta.name] = meta;
		}

		void Deregister(CmptMeta meta)
		{
			cmptDic.erase(meta.name);
		}

		std::vector<CmptMeta> GetCmptMetaVec()
		{
			std::vector<CmptMeta> cmptVec;
			for (auto& [key, meta] : cmptDic) { cmptVec.push_back(meta); }
			return cmptVec;
		}

		void ShowInfo()
		{
			std::string cmptStr =  "Cmpt:  ";
			std::string sizeStr =  "Size:  ";
			std::string alignStr = "Align: ";

			std::vector<CmptMeta> cmptVec = GetCmptMetaVec();
			sort(cmptVec.begin(), cmptVec.end(), [](CmptMeta a, CmptMeta b) {return a.hash <= b.hash; });
			for (auto& meta : cmptVec)
			{
				cmptStr = cmptStr + "    " + meta.name;
				sizeStr = sizeStr + "    " + std::to_string(meta.size);
				alignStr = alignStr + "    " + std::to_string(meta.align);
			}

			std::cout << cmptStr << std::endl;
			std::cout << sizeStr << std::endl;
			std::cout << alignStr << std::endl;	
		}

	private:
		std::unordered_map<std::string, CmptMeta> cmptDic;
		friend class Archetype;
		friend class World;
	};
}