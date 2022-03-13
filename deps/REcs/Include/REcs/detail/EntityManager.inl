#pragma once

namespace REcs
{
	template<typename... Cmpts>
	Entity EntityManager::CreateEntity()
	{
		/*
			Id:Entity's [Id] is the  [inUseEntityTable]'s index
		*/
		size_t newId;

		/*
			If an Entity was Deleted,we put it's [Id] into freeEntityTable.For reusing the [Id] at next Creation;
		*/
		if (!freeEntityTable.empty())		//if have reuse Id，use it
		{
			newId = freeEntityTable.back();
			freeEntityTable.pop_back();
		}
		else
		{
			newId = inUseEntityTable.size();
			inUseEntityTable.emplace_back(EntityData{});
		}

		static_assert(!RTmpl::HasSame<RTmpl::Typelist<Entity, Cmpts...>>, "Cmpts must be different");
		std::vector<CmptMeta> cmptVec = GetCmptMetaVec<Entity, Cmpts...>();

		Archetype* archeType = GetOrCreateArchetypeOf(cmptVec);
		Entity newEnt{ newId,1 };

		EntityData& newEntDat = inUseEntityTable[newId];
		newEntDat.archeType = archeType;
		newEntDat.archGlobalIdx = archeType->CreateArch<Entity, Cmpts...>();
		newEntDat.valid = true;

		Set(newEnt, &newEnt);

		std::cout << "  CreateEntity ID:" << Get<Entity>(newEnt)->Id()<<"   "<< archeType->name  << std::endl;
		std::cout << std::endl;

		return newEnt;
	}


	/*
	设置组件数据
	*/
	template<typename Cmpt>
	void EntityManager::Set(Entity ent, Cmpt* srcCmpt)
	{
		if (ent.version == 0)
		{
			std::cout << "EntityManager::Set-------------This Entity has been Destory" << std::endl;
		}
		else
		{
			EntityData& entDat = inUseEntityTable[ent.Id()];
			if (entDat.valid == false)
			{
				std::cout << "EntityManager::Set-------------Entity Id: " << ent.Id() << "isn't used!" << std::endl;
			}
			else
			{
				Cmpt* dstCmpt = entDat.archeType->GetCmptPtr<Cmpt>(entDat.archGlobalIdx);
				entDat.archeType->cmptsRef.MoveConstruct(RTmpl::Type_SimpleName<Cmpt>(), dstCmpt, srcCmpt);
			}
		}
	}


	/*
	获取组件数据
	*/
	template<typename Cmpt>
	Cmpt* EntityManager::Get(Entity ent)
	{
		if (ent.version == 0)
		{
			std::cout << "EntityManager::Get-------------This Entity has been Destory" << std::endl;
		}
		else
		{
			EntityData& entDat = inUseEntityTable[ent.Id()];
			if (entDat.valid == false)
			{
				std::cout << "EntityManager::Get-------------Entity Id: " << ent.Id() << "isn't used!" << std::endl;
				return nullptr;
			}
			else
			{
				return entDat.archeType->GetCmptPtr<Cmpt>(entDat.archGlobalIdx);
			}
		}
	}

	/*
	添加组件
	1.根据添加组件后的组件TypeList查询或生成新的ArcheType
	2.将原ArcheType里的非增添组件数据搬移到新的ArcheType中
	3.在新的ArcheType中初始化增添的组件
	*/
	template<typename ...Cmpts>
	void EntityManager::Add(Entity ent)
	{
		
		if (ent.version == 0)
		{
			std::cout << "EntityManager::Get-------------This Entity has been Destory" << std::endl;
		}
		else
		{
			/*确认新增组件与以往的组件不重复*/
			static_assert(!RTmpl::HasSame<RTmpl::Typelist<Cmpts...>>, "Cmpts must be different");
			EntityData& entDat = inUseEntityTable[ent.Id()];
			Archetype* oldArcheType = entDat.archeType;
			size_t oldArchGlobalIdx = entDat.archGlobalIdx;

			std::vector<CmptMeta> cmptVec = oldArcheType->cmptsRef.GetCmptMetaVec();
			std::vector<CmptMeta> addVec = GetCmptMetaVec<Cmpts...>();
			std::vector<CmptMeta> resultVec = cmptVec;

			bool cmptSame = false;
			for (auto& newMeta : addVec)
			{
				bool needAdd = true;
				for (auto& oldMeta : cmptVec)
				{
					if (newMeta.name == oldMeta.name)
					{
						needAdd = false;
						break;
					}
				}
				if (needAdd == true)
				{
					resultVec.push_back(newMeta);
				}
			}

			/*
			oldArcheType<A,B>        ->       newArcheType<A,B,C>
			*/
			if (resultVec.size()>cmptVec.size())
			{
				Archetype* newArcheType = GetOrCreateArchetypeOf(resultVec);
				/* newArcheType<A,B,C>:CreateArch  newArch<C> */
				size_t newArchGlobalIdx = newArcheType->CreateArch<Cmpts...>();

				/* move(oldArchType:oldArch<A,B>) to (newArchType:newArch<A,B> Get: newArch<A,B,C>) */
				for (auto& meta : cmptVec)
				{
					newArcheType->cmptsRef.MoveConstruct(meta.name, newArcheType->GetCmptBufferPtr_Str(newArchGlobalIdx, meta.name), oldArcheType->GetCmptBufferPtr_Str(oldArchGlobalIdx, meta.name));
				}
				entDat.archeType = newArcheType;
				entDat.archGlobalIdx = newArchGlobalIdx;

				/* delete oldArch */
				DeleteArch(oldArcheType, oldArchGlobalIdx);
				std::cout << "  Entity ID:" << ent.Id() << "  Add " << oldArcheType->name << "-->" << newArcheType->name << std::endl;
				std::cout << std::endl;
			}
		}
	}
	
	/*
	移除组件
	1.根据删除组件后的剩余组件TypeList查询或生成新的ArcheType
	2.将原ArcheType里的非删除组件数据搬移到新的ArcheType中
	3.在新的ArcheType中初始化增添的组件
	*/
	template<typename ...Cmpts>
	void EntityManager::Remove(Entity ent)
	{
		/*获取剩余组件Vec*/
		static_assert(!RTmpl::HasSame<RTmpl::Typelist<Cmpts...>>, "Cmpts must be different");
		EntityData& entDat = inUseEntityTable[ent.Id()];
		Archetype* oldArcheType = entDat.archeType;
		size_t oldArchGlobalIdx = entDat.archGlobalIdx;

		std::vector<CmptMeta> cmptVec = oldArcheType->cmptsRef.GetCmptMetaVec();
		std::vector<CmptMeta> rmVec = GetCmptMetaVec<Cmpts...>();
		std::vector<CmptMeta> resultVec;

		for (auto& oldMeta : cmptVec)
		{
			bool needrm = false;
			for (auto& rmMeta : rmVec)
			{
				if (oldMeta.name == rmMeta.name)
				{
					needrm = true;
					break;
				}
			}
			if (needrm == false)
				resultVec.push_back(oldMeta);
		}

		/*
			oldArcheType<A,B,C>        ->       newArcheType<A,B>
		*/
		if (resultVec.size() < cmptVec.size())
		{
			Archetype* newArcheType = GetOrCreateArchetypeOf(resultVec);
			/* newArcheType<A,B>:CreateArch  newArch<> */
			size_t newArchGlobalIdx = newArcheType->CreateArch<>();

			/* move(oldArchType:oldArch<A,B>) to (newArchType:newArch<A,B> Get: newArch<A,B>) */
			for (auto& meta : resultVec)
			{
				newArcheType->cmptsRef.MoveConstruct(meta.name, newArcheType->GetCmptBufferPtr_Str(newArchGlobalIdx, meta.name), oldArcheType->GetCmptBufferPtr_Str(oldArchGlobalIdx, meta.name));
			}
			entDat.archeType = newArcheType;
			entDat.archGlobalIdx = newArchGlobalIdx;

			/* delete oldArch */
			DeleteArch(oldArcheType, oldArchGlobalIdx);
			std::cout << "  Entity ID:" << ent.Id() << "  Remove " << oldArcheType->name << "-->" << newArcheType->name << std::endl;
			std::cout << std::endl;
		}
	}
}