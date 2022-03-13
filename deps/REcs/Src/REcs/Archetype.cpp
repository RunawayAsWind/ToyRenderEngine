#include "../Include/REcs/Archetype.h"

namespace REcs
{
	Archetype::Archetype(std::vector<CmptMeta> cmptVec)
	{
		name = GetUniqueName(cmptVec);
		for (auto& meta : cmptVec)
		{
			cmptsRef.Register(meta);
		}

		archNumPerChunk = Chunk::size / cmptsRef.ArchetypeTotalSize() - 1;//Align对齐需要额外的空间，-1留出空间防止溢出
		cmptVec = cmptsRef.GetCmptMetaVec();
		sort(cmptVec.begin(), cmptVec.end(), [](CmptMeta a, CmptMeta b) {return a.align < b.align; });

		//填充记录cmptsBaseOffsetInOneChunk
		size_t curOffset = 0;
		for (const auto& meta : cmptVec)
		{
			curOffset = meta.align * ((curOffset + meta.align - 1) / meta.align);//+align-1是为了向上取整对齐
			cmptsOffsetInChunk[meta.name] = curOffset;
			curOffset += cmptsRef.Sizeof(meta.name) * archNumPerChunk;
		}
#ifdef REcs_Archetype_Debug
		std::cout << std::endl;
		std::cout << "--------------REcs: Archetype: " << GetUniqueName(cmptVec) << " Create -------------" << std::endl;
		std::cout << std::endl;
		std::cout << "          Chunk::size   /  cmptsRef.ArchetypeTotalSize()    =   archNumPerChunk" << std::endl;
		std::cout << "            " << Chunk::size << "                         " << cmptsRef.ArchetypeTotalSize() << "                         " << archNumPerChunk << std::endl;
		std::cout << "alignSort:          ";
		for (auto& meta : cmptVec)
		{
			std::cout  << meta.name << " : " << meta.align << "  ";
		}
		std::cout << std::endl;
		std::cout << "sizeof:          ";
		for (auto& meta : cmptVec)
		{
			std::cout << meta.name << " : " << meta.size << "  ";
		}
		std::cout << std::endl;
		std::cout << "cmptsBaseOffset:    ";
		RTmpl::ShowPairInfo(cmptsOffsetInChunk);
		std::cout << std::endl;
		std::cout << "----------------------------------------------------------" << std::endl;
		std::cout << std::endl;
#endif
	};

	void Archetype::DestructArch(size_t archGlobalIdx)
	{
		for (auto const& [key, meta] : cmptsRef.cmptDic)
		{
			cmptsRef.Destruct(key, GetCmptBufferPtr_Str(archGlobalIdx, key));
		}
	}

	void Archetype::MoveArch(size_t disIdx, size_t srcIdx)
	{
		for (auto const& [key, meta] : cmptsRef.cmptDic)
		{
			cmptsRef.MoveConstruct(key, GetCmptBufferPtr_Str(disIdx, key), GetCmptBufferPtr_Str(srcIdx, key));
		}
	}

	Chunk::byte* Archetype::GetCmptBufferPtr_Str(size_t archGlobalIdx, std::string cmptName)
	{
		if (cmptsRef.cmptDic.count(cmptName) > 0)
		{
			size_t chunkIdx = archGlobalIdx / archNumPerChunk;
			size_t archIdxInChunk = archGlobalIdx % archNumPerChunk;
			Chunk::byte* chunkBufferPtr = chunks[chunkIdx]->Data();
			Chunk::byte* cmptBaseOffset = chunkBufferPtr + cmptsOffsetInChunk[cmptName];
			Chunk::byte* archCmptPtr = cmptBaseOffset + archIdxInChunk * cmptsRef.cmptDic[cmptName].size;
			return archCmptPtr;
		}
		return nullptr;
	}


}