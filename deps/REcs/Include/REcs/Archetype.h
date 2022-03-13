#pragma once
#include <RTmpl/Hash.h>
#include <RTmpl/Container/ShPtrPool.h>
#include "Chunk.h"
#include "CmptsRefMap.h"

namespace REcs 
{
	class Archetype
	{
	public:
		std::string name;
		CmptsRefMap cmptsRef;					//save the Cmpts and Entity's  move copy construct¡¢ destruct;sizeof¡¢align info
		size_t archNumPerChunk = 0;				//the num of Arch that a chunk can contain
		std::unordered_map<std::string, size_t> cmptsOffsetInChunk;		//Cmpt or Entity's start offset in a Chunk

		//ArchType >Chunk > Arch(Cmpts+Entity)
		size_t archTotalNum = 0;				// num of Arch
		std::vector<std::shared_ptr<Chunk>> chunks;
		inline static RTmpl::ShPtrPool<Chunk> chunkPool;

		Archetype(std::vector<CmptMeta> cmptVec);

		template<typename... EntityAndCmpts>
		size_t CreateArch();
		void DestructArch(size_t archGlobalIdx);
		void MoveArch(size_t disIdx, size_t srcIdx);

		template<typename Cmpt>
		Cmpt* GetCmptPtr(size_t archGlobalIdx);
		template<typename Cmpt>
		Chunk::byte* GetCmptBufferPtr(size_t archGlobalIdx);
		Chunk::byte* GetCmptBufferPtr_Str(size_t archGlobalIdx, std::string cmptName);
	};
}

#include "detail/Archetype.inl"