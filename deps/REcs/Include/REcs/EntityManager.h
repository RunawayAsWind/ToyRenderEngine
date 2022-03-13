#pragma once
#include <RTmpl/TmplStruct/Typelist.h>
#include "CmptMeta.h"
#include "Entity.h"
#include "Archetype.h"


/*
The EntityManager provides an API to create, read, update, and destroy entities.
A World has one EntityManager, which manages all the entities for that World.
*/
namespace REcs
{
	struct EntityData
	{
	public:
		Archetype* archeType = nullptr;
		size_t archGlobalIdx = 0;
		bool valid = false;
	};

	class EntityManager
	{
	public:
		template<typename... Cmpts>
		Entity CreateEntity();

		Archetype* GetOrCreateArchetypeOf(std::vector<CmptMeta> cmptVec);

		template<typename Cmpt>
		void Set(Entity ent, Cmpt* srcCmpt);
		
		template<typename Cmpt>
		Cmpt* Get(Entity ent);
		
		template<typename ...Cmpts>
		void Add(Entity ent);
		
		template<typename ...Cmpts>
		void Remove(Entity ent);

		Archetype* GetEntityArType(Entity end);
		void DestoryEntity(Entity ent);
		void DeleteArch(Archetype* archeType, int delArchIdx);
		void ShowInfo();

	private:
		std::vector<EntityData> inUseEntityTable;
		std::vector<size_t> freeEntityTable;
		std::unordered_map<std::string, Archetype*> h2a;

		friend class World;
		EntityManager() {};
		~EntityManager() {};
	};
}

#include "detail/EntityManager.inl"