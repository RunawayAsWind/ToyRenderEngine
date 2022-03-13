#pragma once
#include <iostream>

namespace RTmpl
{
	template<typename Tuple, typename Func,typename IdxType, IdxType... idxs>
	void TransTupleDo(Tuple tp, Func func,std::integer_sequence<IdxType, idxs...> seq)
	{
		((func(std::get<idxs>(tp))), ...);
	}

	template<typename Func,typename...Args>
	void TransTupleDo(std::tuple<Args...> tp, Func func)
	{
		TransTupleDo(tp, func,std::make_index_sequence<sizeof...(Args)>());
	}

	template<typename...Args>
	struct TupleList
	{
		constexpr TupleList(Args...args) : tp{ args... }{}
		std::tuple<Args...> tp;
	};
	
	template<typename...Args>
	struct FieldList :public TupleList<Args...>
	{
		constexpr FieldList(Args...args) :TupleList<Args...>{ args... } {}
	};

	template<typename ClassPtr,typename...Args >
	struct Field :public TupleList<Args...>
	{
	public:
		constexpr Field(const char* name,ClassPtr ptr,Args...args) : name{ name },ptr(ptr),TupleList<Args...>{ args... } {}
		constexpr std::string GetName() { return name.data(); }
		std::string_view name;
		ClassPtr ptr;
	};

	enum class ClassMemType
	{
		isValue,
		isFunc,
	};

	template<typename classType, typename valType>
	ClassMemType GetClassTraits(valType classType::* classVal)
	{
		return ClassMemType::isValue;
	}

	template<typename classType,typename returnType,typename...Args>
	ClassMemType GetClassTraits(returnType (classType::*classFunc)(Args...))
	{
		return ClassMemType::isFunc;
	}

	template<typename classType>
	class Reflection {
	public:
		template<typename Func>
		static void DoForEachVal(Func func) {}
	};
}