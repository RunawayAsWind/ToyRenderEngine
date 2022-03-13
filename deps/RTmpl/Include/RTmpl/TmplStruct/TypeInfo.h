#pragma once
#include "../Hash.h"

namespace RTmpl 
{
	class TypeInfo
	{
	public:
		template<typename T>
		constexpr TypeInfo(T t) : hash(Type_Hash<T>()), name(NAMEOF_TYPE(T)),simpleName(RTmpl::Type_SimpleName<T>()),size(sizeof(T)),align(alignof(T)) {};
		constexpr hash_t Hash() const noexcept { return hash; }
		std::string Name() const noexcept { return name; }
		std::string SimpleName() const noexcept { return simpleName; }
		constexpr size_t Size() const noexcept { return size; }
		constexpr size_t Align() const noexcept { return align; }
		void ShowInfo() { std::cout << "Name: " << name << "  SimpleName: " << simpleName << "   Hash: " << hash << "  Size: " << size << "  Align: " << align << std::endl; };
	private:
		hash_t hash;
		std::string name;
		std::string simpleName;
		size_t size;
		size_t align;
	};
}