#pragma once

namespace REcs
{
	template<typename... EntityAndCmpts>
	size_t Archetype::CreateArch()
	{
		if (archTotalNum == chunks.size() * archNumPerChunk)		//һ��chunksֻ��װ��chunkCapacity��Arch,������������ޣ�����µ�chunk
		{
			auto chunk = chunkPool.Get();
			chunks.push_back(chunk);
		}
		size_t archGlobalIdx = archTotalNum;
		archTotalNum++;

		(new(GetCmptBufferPtr<EntityAndCmpts>(archGlobalIdx))EntityAndCmpts, ...);	//�Ը���������ڴ洦����������캯����ʼ�����
		return archGlobalIdx;
	}

	template<typename Cmpt>
	Cmpt* Archetype::GetCmptPtr(size_t archGlobalIdx)
	{
		return reinterpret_cast<Cmpt*>(GetCmptBufferPtr<Cmpt>(archGlobalIdx));
	}

	template<typename Cmpt>
	Chunk::byte* Archetype::GetCmptBufferPtr(size_t archGlobalIdx)
	{
		return GetCmptBufferPtr_Str(archGlobalIdx, RTmpl::Type_SimpleName<Cmpt>());
	}
}
