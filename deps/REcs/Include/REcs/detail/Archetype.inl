#pragma once

namespace REcs
{
	template<typename... EntityAndCmpts>
	size_t Archetype::CreateArch()
	{
		if (archTotalNum == chunks.size() * archNumPerChunk)		//一个chunks只能装下chunkCapacity个Arch,如果到达总上限，添加新的chunk
		{
			auto chunk = chunkPool.Get();
			chunks.push_back(chunk);
		}
		size_t archGlobalIdx = archTotalNum;
		archTotalNum++;

		(new(GetCmptBufferPtr<EntityAndCmpts>(archGlobalIdx))EntityAndCmpts, ...);	//对各组件所在内存处调用组件构造函数初始化组件
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
