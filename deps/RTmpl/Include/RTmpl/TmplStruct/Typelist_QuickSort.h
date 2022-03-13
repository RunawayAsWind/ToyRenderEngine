#pragma once
#include "Typelist.h"
#include "../Hash.h"
namespace RTmpl
{
    /*
    -------------      TypeList Content QuikSort at Compile time          ------------------
            (Some Code from https://gist.github.com/goldshtn/ea610da529ecb2fc1af5)
         We need to Sort the Contents of TypeList ,for satisfy that “Hash Not related to the Order of Typelist's content”（from Purpose 2）
     */


     /*
         Predicate（谓语）:指大于、小于、等于、等元素级别操作

         使用多层模板嵌套实现Predicate（谓语）
         便于对某部分模板的单独实现

         例如，比较两个未知类型的大小的谓语，公式为   T1>T2?    (表示两个待传入类型T1,T2的比较)

         分层实现,对T2赋值Type,公式变为    T1>Type?   (待传入类型T1与已赋值的Type进行比较)
     */

    class Pred_TypeSize_Bigger
    {
    public:
        template<typename Right>
        class SetRight
        {
        public:
            template<typename Left>
            class SetLeft
            {
            public:
                static constexpr bool value = (sizeof(Left) >= sizeof(Right));
            };
        };
    };
    /*

    仅仅用sizeof进行大小比较，可能会出现这种情况
    Typelist<int,string,long> 快排得到 Typelist<int,long,string>
    Typelist<long,string,int> 快排得到 Typelist<long,int,string>
    这是因为sizeof(int)=sizeof(long)造成的

    但显然这不是我们希望的，否则我们添加组件还要考虑组件顺序

    改进思路：寻求一个值，不同Type的值是唯一的，利用这个值代替size进行排序

    经资料查找后，发现可以采用编译时typeid，根据typeid生成的hash码

    sizeof(int)=sizeof(long)
    but
    typeid(int).hash()!=typeid(long).hash()(所有类型的typeid都不相同)

    */

    class Pred_TypeID_Bigger
    {
    public:
        template<typename Right>
        class SetRight
        {
        public:
            template<typename Left>
            class SetLeft
            {
            public:
                static constexpr bool value = (Type_Hash<Left>() >= Type_Hash<Right>());
            };
        };
    };



    /*
    回顾一下快排的单个周期
    1.“哨兵”选取
    2.小于哨兵的放左侧，大于哨兵的放右侧

    可见  对于谓语T1>T2 快排的周期谓语为 SetRight赋值 T1>哨兵
    */
    template<typename Pred, typename Head>
    class PredSetRight_T
    {
    public:
        using Type = typename Pred::template SetRight<Head>;
    };

    template<typename Pred, typename Head>
    using PredSetRight = typename PredSetRight_T<Pred, Head>::Type;     //如果Pred传入的是 Pred_TypeSize_Bigger 此时PredSetRight 表示公式 Left>=Head？






    //Predicate(谓语)的否定转义
    template<typename PredSetRight>
    class PredSetRight_Negate
    {
    public:
        template<typename Left>     //仅在右值赋值，左值尚未赋值的情况下转义，因此要给出设置左值的接口
        class SetLeft
        {
        public:
            static constexpr bool value = !(PredSetRight::template  SetLeft<Left>::value); //如果Pred传入的是 PredSetRight<Pred_TypeSize_Bigger,Head> 此时PredSetRight_Negate 表示 Left<HEAD？
        };
    };





    /*
        对每一串元素进行处理
    */
    template<typename Pred, typename List>
    class UsePredtoList_T;

    template<typename Pred, typename List>
    using UsePredtoList = typename UsePredtoList_T<Pred, List>::result;

    template<typename Pred, typename List>
    class UsePredtoList_T
    {
    public:
        using trueType = ListCat< Typelist<Front<List>>, UsePredtoList<Pred, PopFront<List>> >;//对应快排中的，如果当前元素 大于/小于 哨兵，丢入 右侧/左侧
        using falseType = UsePredtoList<Pred, PopFront<List>>;
        using result = IfThenElse<Pred::template SetLeft<Front<List>>::value, trueType, falseType>;
    };

    template<typename Pred>
    class UsePredtoList_T<Pred, Typelist<>>
    {
    public:
        using result = Typelist<>;
    };





    template<typename Pred, typename List>
    class Typelist_Sort_T;

    template <typename Pred, typename List>
    using Typelist_Sort = typename Typelist_Sort_T<Pred, List>::type;

    template <typename Pred, typename Head, typename... Tail>
    class Typelist_Sort_T<Pred, Typelist<Head, Tail...>>
    {
    public:
        using predicate = PredSetRight<Pred, Head>;
        using notpredicate = PredSetRight_Negate<predicate>;
        using smaller_than = UsePredtoList<notpredicate, Typelist<Tail...>>;
        using greater_than = UsePredtoList<predicate, Typelist<Tail...>>;
        using type = ListCat<
            Typelist_Sort<Pred, smaller_than>,
            Typelist<Head>,
            Typelist_Sort<Pred, greater_than>
        >;
    };

    template <typename Pred>
    class Typelist_Sort_T<Pred, Typelist<>>
    {
    public:
        using type = Typelist<>;
    };

    template<typename List>
    using ListQuickSortBySize = Typelist_Sort<Pred_TypeSize_Bigger, List>;


    template<typename List>
    using ListQuickSortByID = Typelist_Sort<Pred_TypeID_Bigger, List>;
}