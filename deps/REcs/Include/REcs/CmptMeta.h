#pragma once
#include <RTmpl/Hash.h>
#include <functional>
#include <iostream>

namespace REcs 
{
	struct CmptMeta
	{
		std::string name;
		RTmpl::hash_t hash;
		size_t size;
		size_t align;
		std::function<void(void*, void*)> copyConstr = nullptr;
		std::function<void(void*, void*)> moveConstr = nullptr;
		std::function<void(void*)> destr = nullptr;

		template<typename Cmpt>
		void Record()
		{
			static_assert(std::is_copy_constructible_v<Cmpt>, "<Cmpt> must be copy-constructible");
			static_assert(std::is_move_constructible_v<Cmpt>, "<Cmpt> must be move-constructible");
			static_assert(std::is_destructible_v<Cmpt>, "<Cmpt> must be destructible");

			name = RTmpl::Type_SimpleName<Cmpt>();
			hash = RTmpl::Type_Hash<Cmpt>();
			size = sizeof(Cmpt);
			align = alignof(Cmpt);
			if constexpr (!std::is_trivially_destructible_v<Cmpt>) {
				destr = [](void* cmpt) {
					reinterpret_cast<Cmpt*>(cmpt)->~Cmpt();
				};
			}
			if constexpr (!std::is_trivially_move_constructible_v<Cmpt>) {
				moveConstr = [](void* dst, void* src) {
					new(dst)Cmpt(std::move(*reinterpret_cast<Cmpt*>(src)));
				};
			}
			if constexpr (!std::is_trivially_copy_constructible_v<Cmpt>) {
				copyConstr = [](void* dst, void* src) {
					new(dst)Cmpt(*reinterpret_cast<Cmpt*>(src));
				};
			}
		}

		void ShowInfo()
		{
			std::cout << "name: " << name << "size: " << size << "align: " << align << std::endl;
		}
	};


	template<typename Cmpt>
	void AddMeta(std::vector<CmptMeta>& cmptVec)
	{
		CmptMeta meta;
		meta.Record<Cmpt>();
		cmptVec.push_back(meta);
	}

	template<typename... Cmpts>
	std::vector<CmptMeta> GetCmptMetaVec()
	{
		std::vector<CmptMeta> cmptVec;
		(AddMeta<Cmpts>(cmptVec), ...);
		return cmptVec;
	}

	std::string GetUniqueName(std::vector<CmptMeta> cmptVec);

}