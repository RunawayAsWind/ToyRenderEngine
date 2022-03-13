#pragma once

/*
	IfThenElse<true,a,b> = a;
	IfThenElse<false,a,b> = b;
*/
namespace RTmpl
{
	template<bool COND, typename TrueType, typename FalseType>
	struct IfThenElseT { using Type = TrueType; };

	template<typename TrueType, typename FalseType>
	struct IfThenElseT<false, TrueType, FalseType>
	{
		using Type = FalseType;
	};

	template<bool COND, typename TrueType, typename FalseType>
	using IfThenElse = typename IfThenElseT<COND, TrueType, FalseType>::Type;
}
