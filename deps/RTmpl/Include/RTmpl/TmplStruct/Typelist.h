#pragma once
#include "../Hash.h"
#include "../Traits.h"

namespace RTmpl
{
	template<typename... T>
	class Typelist {};


	/*
		PopFront<Typelist<a,b,c>> = Typelist<b,c>
	*/
	template<typename List>
	class PopFrontT;

	template<typename Head, typename... Tail>
	class PopFrontT<Typelist<Head, Tail...>>
	{
		public:
			using Type = Typelist<Tail...>;
	};

	template<typename List>
	using PopFront = typename PopFrontT<List>::Type;


	/*
		Front<Typelist<a,b,c>> = a
	*/
	template<typename List>
	class FrontT;

	template<typename Head, typename... Tail>
	class FrontT<Typelist<Head, Tail...>>
	{
		public:
			using Type = Head;
	};

	template<typename List>
	using Front = typename FrontT<List>::Type;


	/*
		PushFront<Typelist<a,b,c>,d> = Typelist<d,a,b,c>
	*/
	template<typename List, typename NewElement>
	class PushFrontT;

	template<typename... Elements, typename NewElement>
	class PushFrontT<Typelist<Elements...>, NewElement>
	{
	public:
		using Type = Typelist<NewElement, Elements...>;
	};

	template<typename List, typename NewElement>
	using PushFront = typename PushFrontT<List, NewElement>::Type;


	/*
		PushBack<Typelist<a,b,c>,d> = Typelist<a,b,c,d>
	*/
	template<typename List, typename NewElement>
	class PushBackT; 

	template<typename... Elements, typename NewElement>
	class PushBackT<Typelist<Elements...>, NewElement>
	{ 
	public:
		using Type = Typelist<Elements..., NewElement>; 
	}; 
	
	template<typename List, typename NewElement>
	using PushBack = typename PushBackT<List, NewElement>::Type;


	/*
		IsEmpty<Typelist<>>		= true
		IsEmpty<Typelist<a>>	= false
	*/
	template<typename List>
	class IsEmptyT;

	template<typename ...T>
	class IsEmptyT<Typelist<T...>>
	{
		public:
			static constexpr bool value = false;
	};

	template<>
	class IsEmptyT<Typelist<>>
	{
		public:
			static constexpr bool value = true;
	};

	template<typename List>
	constexpr bool IsEmpty = IsEmptyT<List>::value;


	/*
		IsHomogeneous<Typelist<>>		= false
		IsHomogeneous<Typelist<a>>		= true
		IsHomogeneous<Typelist<a,b,b>>	= false
		IsHomogeneous<Typelist<a,a,a>>	= true
		else = false
	*/
	template<typename List>
	class IsHomogeneousT;

	template<>
	class IsHomogeneousT<Typelist<>>
	{
	public:
		static constexpr bool value = false;
	};

	template<typename T>
	class IsHomogeneousT<Typelist<T>>
	{
	public:
		static constexpr bool value = true;
	};

	template<typename T1, typename... TN>
	class IsHomogeneousT<Typelist<T1, TN...>>
	{
		public:
			static constexpr bool value = (std::is_same<T1, TN>::value && ...);
	};

	template<typename List>
	constexpr bool IsHomogeneous = IsHomogeneousT<List>::value;
	
	/*
		FirstSame<Typelist<>>		=	false
		FirstSame<Typelist<a>>		=   false
		FirstSame<Typelist<a,b,b>>	=   false
		FirstSame<Typelist<a,b,a>>	=   true
   */
	template<typename List>
	class FirstSameT;

	template<>
	class  FirstSameT<Typelist<>>
	{
		public:
			static constexpr bool value = false;
	};

	template<typename T>
	class  FirstSameT<Typelist<T>>
	{
		public:
			static constexpr bool value = false;
	};

	template<typename T1, typename... TN>
	class FirstSameT<Typelist<T1, TN...>>
	{
		public:
			static constexpr bool value = (std::is_same<T1, TN>::value || ...);
	};

	template<typename List>
	constexpr bool FirstSame = FirstSameT<List>::value;


	/*
		HasSame<Typelist<>>			=	false;
		HasSame<Typelist<a>>		=	false;
		HasSame<Typelist<a,b,c>>	=	false;
		HasSame<Typelist<a,b,b>>	=	true;
   */
	template<typename List>
	class HasSameT;

	template<>
	class  HasSameT<Typelist<>>
	{
		public:
			static constexpr bool value = false;
	};

	template<typename T>
	class  HasSameT<Typelist<T>>
	{
		public:
			static constexpr bool value = false;
	};

	template<typename T1, typename ...T2>
	class HasSameT<Typelist<T1, T2...>>
	{
		public:
			static constexpr bool value = FirstSame<Typelist<T1, T2...>> || FirstSame<Typelist<T2...>>;
	};

	template<typename List>
	constexpr bool HasSame = HasSameT<List>::value;


	/*
		ListCat<Typelist<>>					=	Typelist<>
		ListCat<Typelist<>,Typelist<a>>		=	Typelist<a>
		ListCat<Typelist<a>>				=	Typelist<a>
		ListCat<Typelist<a>,Typelist<b>>	=	Typelist<a,b>
	*/
	template<typename ...List>
	class ListCatT;

	template<typename ...T>
	class ListCatT<Typelist<T...>>
	{
		public:
			using Type = Typelist<T...>;
	};

	template<typename ...T1, typename ...T2>
	class ListCatT<Typelist<T1...>, Typelist<T2...>>
	{
		public:
			using Type = Typelist<T1..., T2...>;
	};

	template<typename ...T1, typename ...T2, typename ...T3>
	class ListCatT<Typelist<T1...>, Typelist<T2...>, T3...>
	{
		public:
			using Type = typename ListCatT<Typelist<T1..., T2...>, T3...>::Type;
	};

	template<typename ...T>
	using ListCat = typename ListCatT<T...>::Type;



	/*
		ListUnpackFirst< Typelist<> >				=	Typelist<>
		ListUnpackFirst< Typelist<a> >				=	Typelist<a>
		ListUnpackFirst< Typelist<Typelist<a>,b> >	=	Typelist<a,b>
		ListUnpackFirst< Typelist<Typelist<a>,b,c> >	=	Typelist<a,b,c>
		ListUnpackFirst< Typelist<Typelist<Typelist<a>,b>,c> >	=	Typelist<a,b,c>
		ListUnpackFirst< Typelist<a,Typelist<b>> >	Error
	*/
	template<typename List>
	class ListUnpackFirstT;

	template<typename ...T>
	class ListUnpackFirstT<Typelist<T...>>
	{
		public:
			using Type = Typelist<T...>;
	};

	template<typename ...Head, typename ...Tail>
	class ListUnpackFirstT<Typelist<Typelist<Head...>, Tail...>>
	{
		public:
			using Type = ListCat< typename ListUnpackFirstT<Typelist<Head...>>::Type, Typelist<Tail...>>;
	};

	template<typename List>
	using ListUnpackFirst = typename ListUnpackFirstT<List>::Type;



	/*
		ListUnpackFirst< Typelist<> >				=	Typelist<>
		ListUnpackFirst< Typelist<a> >				=	Typelist<a>
		ListUnpackFirst< Typelist<Typelist<a>,b> >	=	Typelist<a,b>
		ListUnpackFirst< Typelist<Typelist<a>,b,Typelist<c>> >	=	Typelist<a,b,c>
		ListUnpackFirst< Typelist< Typelist<a>,b,Typelist<c,Typelist<d>> > >	=	Typelist<a,b,c,d>
	*/
	template<typename T>
	class ListUnpackT
	{
	public:
		using Type = Typelist<T>;
	};

	template<>
	class ListUnpackT<Typelist<>>
	{
	public:
		using Type = Typelist<>;
	};

	template<typename Head, typename ...Tail>
	class ListUnpackT<Typelist<Head, Tail...>>
	{
	public:
		using Type = ListCat < typename ListUnpackT<Head>::Type, typename ListUnpackT<Typelist<Tail...>>::Type >;
	};

	template<typename List>
	using ListUnpack = typename ListUnpackT<List>::Type;



	/*
		ListContain< Typelist<>,a > = false;
		ListContain< Typelist<a,b,c>,a > = true;
	*/
	template<typename List, typename SearchGoal>
	class ListContainT;

	template<typename SearchGoal>
	class ListContainT<Typelist<>, SearchGoal>
	{
		public:
			static constexpr bool res = false;
	};

	template<typename Head, typename...Tail, typename SearchGoal>
	class ListContainT<Typelist<Head, Tail...>, SearchGoal>
	{
		public:
			static constexpr bool res = (NAMEOF_FULL_TYPE(Head) == NAMEOF_FULL_TYPE(SearchGoal)) || ListContainT<Typelist<Tail...>, SearchGoal>::res;
	};

	template<typename List,typename SearchGoal>
	constexpr bool ListContain = ListContainT<List, SearchGoal>::res;



	/*
		ListInject< Typelist<>, Typelist<> >=Typelist<>
		ListInject< Typelist<>, Typelist<a,b> >=Typelist<>
		ListInject< Typelist<a>, Typelist<a,b> >=Typelist<a>
	*/
	template<typename ResList, typename List1, typename List2>
	class ListInjectT;

	template<typename...T1, typename...T3>
	class ListInjectT<Typelist<T1...>, Typelist<>, Typelist<T3...>>
	{
		public:
			using Type = Typelist<T1...>;
	};

	template<typename...T1, typename Head, typename...Tail, typename...T3>
	class ListInjectT<Typelist<T1...>, Typelist<Head, Tail...>, Typelist<T3...>>
	{
		public:
			using Type = typename ListInjectT<IfThenElse< (!ListContain<Typelist<T1...>, Head> && ListContain<Typelist<T3...>, Head>), PushBack<Typelist<T1...>, Head>, Typelist<T1...>>, Typelist<Tail...>, Typelist<T3...>>::Type;
	};

	template<typename List1,typename List2>
	using ListInject = typename ListInjectT<Typelist<>, List1, List2>::Type;


	/*
		ListInjectMul< Typelist<a,b,c,d>, Typelist<a,b,c>, Typelist<a,c> >=Typelist<a,c>
	*/
	template<typename... List>
	class ListInjectMulT;

	template<typename...T1,typename... T2>
	class ListInjectMulT<Typelist<T1...>, Typelist<T2...>>
	{
		public:
			using Type = ListInject<Typelist<T1...>, Typelist<T2...>>;
	};

	template<typename List1,typename List2,typename ...List3>
	class ListInjectMulT<List1,List2,List3...>
	{
		public:
			using Type = typename ListInjectMulT< ListInject<List1, List2>, List3...>::Type;
	};

	template<typename... List>
	using ListInjectMul = typename ListInjectMulT<List...>::Type;

	/*
		ListUnion< Typelist<>, Typelist<> >=Typelist<>
		ListUnion< Typelist<>, Typelist<a,b> >=Typelist<a,b>
		ListUnion< Typelist<a,c>, Typelist<a,b> >=Typelist<a,b,c>
	*/
	template<typename ResList, typename MergeList>
	class ListUnionT;

	template<typename...T1>
	class ListUnionT<Typelist<T1...>, Typelist<>>
	{
		public:
			using Type = Typelist<T1...>;
	};

	template<typename...T1, typename Head,typename... Tail>
	class ListUnionT<Typelist<T1...>, Typelist<Head,Tail...>>
	{
		public:
			using Type = typename ListUnionT< IfThenElse< !ListContain<Typelist<T1...>,Head>, PushBack<Typelist<T1...>, Head>, Typelist<T1...> >, Typelist<Tail...>>::Type;
	};

	template<typename List1, typename List2>
	using ListUnion = typename ListUnionT < Typelist<>, ListCat<List1,List2> >::Type;


	/*
		ListUnionMul< Typelist<a,b,c,d>, Typelist<a,b,c>, Typelist<a,c> >=Typelist<a,bc,d>
	*/
	template<typename... List>
	class ListUnionMulT;

	template<typename...T1, typename... T2>
	class ListUnionMulT<Typelist<T1...>, Typelist<T2...>>
	{
		public:
			using Type = ListUnion<Typelist<T1...>, Typelist<T2...>>;
	};

	template<typename List1, typename List2, typename ...List3>
	class ListUnionMulT<List1, List2, List3...>
	{
		public:
			using Type = typename ListUnionMulT< ListUnion<List1, List2>, List3...>::Type;
	};

	template<typename... List>
	using ListUnionMul = typename ListUnionMulT<List...>::Type;
}