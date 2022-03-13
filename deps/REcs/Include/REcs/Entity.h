#pragma once

/*
Entities are one of the three principle elements of an Entity Component System architecture. 
They represent the individual "things" in your game or application.
An entity is essentially an ID.
*/
namespace REcs
{
	class Entity
	{
	public:
		size_t Id() const noexcept { return id; }
		size_t Version() const noexcept { return version; }
		constexpr Entity(size_t id = 0, size_t version = 0) : id(id), version(version) {}
	private:
		friend class EntityManager;
		friend class Archetype;
		size_t id;
		size_t version;
	};
}