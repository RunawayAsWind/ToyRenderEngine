#include "../Include/REcs/EntityManager.h"
#include <iostream>
using namespace std;

namespace REcs 
{

	Archetype* EntityManager::GetOrCreateArchetypeOf(std::vector<CmptMeta> cmptVec)
	{
		std::string archName = GetUniqueName(cmptVec);
		auto target = h2a.find(archName);

		if (target != h2a.end())
		{
			return target->second;
		}
		else
		{
			Archetype* archetype = new Archetype(cmptVec);
			h2a[archName] = archetype;
			return archetype;
		}
	}

	Archetype* EntityManager::GetEntityArType(Entity ent)
	{
		if (ent.version == 0)
		{
			std::cout << "EntityManager::DestoryEntity-------------This Entity has been Destory" << std::endl;
			return nullptr;
		}
		else
			return inUseEntityTable[ent.Id()].archeType;
	}

	void EntityManager::DestoryEntity(Entity ent)
	{
		if (ent.version == 0)
		{
			std::cout << "EntityManager::DestoryEntity-------------This Entity has been Destory" << std::endl;
		}
		else
		{
			EntityData& entDat = inUseEntityTable[ent.Id()];
			if (entDat.valid == false)
			{
				std::cout << "EntityManager::DestoryEntity-------------Entity Id: " << ent.Id() << "isn't used!" << std::endl;
			}
			else
			{
				/*
				
				DestoryEntity           Entity1:ArchGlobalIdx=2;    ArcheType:<A,B,C>
				
				<A,B,C>->archTotalNum - 1 = 6 
				<A,B,C>->GetCmptPtr<Entity>(6)->Id()=8;
				lastArchBelongEntity    Entity8:ArchGlobalIdx=6;    ArcheType:<A,B,C>


				Before Delete
				ArcheType:<A,B,C>:																														 last								  last							      last
				  Arch1.A  Arch2.A  Arch3.A  Arch4.A  Arch1.B  Arch2.B  Arch3.B  Arch4.B  Arch1.C  Arch2.C  Arch3.C  Arch4.C                  Arch5.A    Arch6.A  Arch7.A  Arch8.A  Arch5.B  Arch6.B  Arch7.B  Arch8.B  Arch5.C  Arch6.C  Arch7.C  Arch8.C   
				||        |Entity1|        |       ||        | Entity1|        |        ||        |Entity1|        |        ||				||Entity7 | Entity8 |        |       ||Entity7 | Entity8 |       |        ||Entity7 | Entity8 |      |        ||
				！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！------------------------------------------			  ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！------------------------------------------
				|                                                 Chunk1                                                    |  			|                                                  Chunk1                                                    | 

				After Delete

				ArcheType:<A,B,C>:																											 last                                   last								 last	
				  Arch1.A  Arch2.A  Arch3.A  Arch4.A  Arch1.B  Arch2.B  Arch3.B  Arch4.B  Arch1.C  Arch2.C  Arch3.C  Arch4.C                  Arch5.A    Arch6.A  Arch7.A  Arch8.A  Arch5.B  Arch6.B  Arch7.B  Arch8.B  Arch5.C  Arch6.C  Arch7.C  Arch8.C
				||        |Entity8|        |       ||        | Entity8|        |        ||        |Entity8|        |        ||				||Entity7 |         |        |       ||Entity7 |        |        |        ||Entity7 |        |        |       ||
				！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！------------------------------------------			  ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！------------------------------------------
				|                                                 Chunk1                                                    |  			|                                                  Chunk1                                                    |

				DestoryEntity           Entity1:ArchGlobalIdx=-1;    ArcheType:None
				lastArchBelongEntity    Entity8:ArchGlobalIdx=2;     ArcheType:<A,B,C>
				*/

				DeleteArch(entDat.archeType, entDat.archGlobalIdx);

				entDat.archeType = nullptr;
				entDat.archGlobalIdx = 0;
				entDat.valid = false;
				freeEntityTable.push_back(ent.Id());

				std::cout << "FreeEntityTable:      ";
				for (int i = 0; i < freeEntityTable.size(); i++)
				{
					std::cout << freeEntityTable[i] << " ";
				}
				std::cout << std::endl;
			}
			ent.version = 0;
		}
	}

	/*
	DeleteArch
	1.destructArch
	2.move last Arch to delete place
	3.archTotalNum--
	*/
	void  EntityManager::DeleteArch(Archetype* archeType, int delArchIdx)
	{
		size_t lastArchIdx = archeType->archTotalNum - 1;
		archeType->DestructArch(delArchIdx);
		if (delArchIdx != lastArchIdx)
		{	
			archeType->MoveArch(delArchIdx, lastArchIdx);
			size_t LastArchEntityId = archeType->GetCmptPtr<Entity>(lastArchIdx)->Id();
			inUseEntityTable[LastArchEntityId].archGlobalIdx = delArchIdx;
		}
		archeType->archTotalNum--;
	}

	void  EntityManager::ShowInfo()
	{
		std::cout << "h2a:            ";
		for (auto& [key,archeType]:h2a)
		{
			cout << archeType->name << "(" << archeType->chunks.size() << "," << archeType->archTotalNum << ")  ";
		}
		std::cout << std::endl;

		std::cout << "ID:             ";
		for (int i = 0; i < inUseEntityTable.size(); i++)
		{
			size_t id = i;
			std::cout << id << " ";
		}
		std::cout << std::endl;

		std::cout << "Archetype:      ";
		for (int i = 0; i < inUseEntityTable.size(); i++)
		{
			string name = "None";
			if (inUseEntityTable[i].archeType != nullptr)
			{
				name = inUseEntityTable[i].archeType->name;
			}
			std::cout << name << " ";
		}
		std::cout << std::endl;

		std::cout << "ArchGlobalIdx:   ";
		for (int i = 0; i < inUseEntityTable.size(); i++)
		{
			std::cout << inUseEntityTable[i].archGlobalIdx << " ";
		}
		std::cout << std::endl;

		std::cout << "IDinUse:        ";
		for (int i = 0; i < inUseEntityTable.size(); i++)
		{
			std::cout << inUseEntityTable[i].valid << " ";
		}
		std::cout << std::endl;

		std::cout << "FreeEntityTable:      ";
		for (int i = 0; i < freeEntityTable.size(); i++)
		{
			std::cout << freeEntityTable[i] << " ";
		}
		std::cout << std::endl;

		std::cout << std::endl;
	}
}