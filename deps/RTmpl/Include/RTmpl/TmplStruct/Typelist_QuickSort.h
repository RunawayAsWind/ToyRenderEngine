#pragma once
#include "Typelist.h"
#include "../Hash.h"
namespace RTmpl
{
    /*
    -------------      TypeList Content QuikSort at Compile time          ------------------
            (Some Code from https://gist.github.com/goldshtn/ea610da529ecb2fc1af5)
         We need to Sort the Contents of TypeList ,for satisfy that ��Hash Not related to the Order of Typelist's content����from Purpose 2��
     */


     /*
         Predicate��ν�:ָ���ڡ�С�ڡ����ڡ���Ԫ�ؼ������

         ʹ�ö��ģ��Ƕ��ʵ��Predicate��ν�
         ���ڶ�ĳ����ģ��ĵ���ʵ��

         ���磬�Ƚ�����δ֪���͵Ĵ�С��ν���ʽΪ   T1>T2?    (��ʾ��������������T1,T2�ıȽ�)

         �ֲ�ʵ��,��T2��ֵType,��ʽ��Ϊ    T1>Type?   (����������T1���Ѹ�ֵ��Type���бȽ�)
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

    ������sizeof���д�С�Ƚϣ����ܻ�����������
    Typelist<int,string,long> ���ŵõ� Typelist<int,long,string>
    Typelist<long,string,int> ���ŵõ� Typelist<long,int,string>
    ������Ϊsizeof(int)=sizeof(long)��ɵ�

    ����Ȼ�ⲻ������ϣ���ģ�����������������Ҫ�������˳��

    �Ľ�˼·��Ѱ��һ��ֵ����ͬType��ֵ��Ψһ�ģ��������ֵ����size��������

    �����ϲ��Һ󣬷��ֿ��Բ��ñ���ʱtypeid������typeid���ɵ�hash��

    sizeof(int)=sizeof(long)
    but
    typeid(int).hash()!=typeid(long).hash()(�������͵�typeid������ͬ)

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
    �ع�һ�¿��ŵĵ�������
    1.���ڱ���ѡȡ
    2.С���ڱ��ķ���࣬�����ڱ��ķ��Ҳ�

    �ɼ�  ����ν��T1>T2 ���ŵ�����ν��Ϊ SetRight��ֵ T1>�ڱ�
    */
    template<typename Pred, typename Head>
    class PredSetRight_T
    {
    public:
        using Type = typename Pred::template SetRight<Head>;
    };

    template<typename Pred, typename Head>
    using PredSetRight = typename PredSetRight_T<Pred, Head>::Type;     //���Pred������� Pred_TypeSize_Bigger ��ʱPredSetRight ��ʾ��ʽ Left>=Head��






    //Predicate(ν��)�ķ�ת��
    template<typename PredSetRight>
    class PredSetRight_Negate
    {
    public:
        template<typename Left>     //������ֵ��ֵ����ֵ��δ��ֵ�������ת�壬���Ҫ����������ֵ�Ľӿ�
        class SetLeft
        {
        public:
            static constexpr bool value = !(PredSetRight::template  SetLeft<Left>::value); //���Pred������� PredSetRight<Pred_TypeSize_Bigger,Head> ��ʱPredSetRight_Negate ��ʾ Left<HEAD��
        };
    };





    /*
        ��ÿһ��Ԫ�ؽ��д���
    */
    template<typename Pred, typename List>
    class UsePredtoList_T;

    template<typename Pred, typename List>
    using UsePredtoList = typename UsePredtoList_T<Pred, List>::result;

    template<typename Pred, typename List>
    class UsePredtoList_T
    {
    public:
        using trueType = ListCat< Typelist<Front<List>>, UsePredtoList<Pred, PopFront<List>> >;//��Ӧ�����еģ������ǰԪ�� ����/С�� �ڱ������� �Ҳ�/���
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