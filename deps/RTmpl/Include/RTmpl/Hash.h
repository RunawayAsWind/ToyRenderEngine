#pragma once
#include "../../3rdParty/nameof/nameof.hpp"
#include "STLHelp.h"
#include <algorithm>

namespace RTmpl
{
    template<typename T>
    std::string Type_SimpleName()
    {
        std::string str(NAMEOF_TYPE(T));
        str = StrReplaceAll<std::string> (str, "class ", "");
        str = StrReplaceAll<std::string>(str, "struct ", "");
        return str;
    }

    template<typename T>
    std::string Type_SimpleName(T t)
    {
        return Type_SimpleName<T>();
    }

    // From https://github.com/foonathan/string_id. As usually, thanks Jonathan.
    using hash_t = std::uint64_t;

    // See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
    constexpr hash_t fnv_basis = 14695981039346656037ull;
    constexpr hash_t fnv_prime = 1099511628211ull;

    // FNV-1a 64 bit hash
    constexpr hash_t fnv1a_hash(std::size_t n, const char* str, hash_t hash = fnv_basis)
    {
        return n > 0 ? fnv1a_hash(n - 1, str + 1, (hash ^ *str) * fnv_prime) : hash;
    }

    constexpr hash_t fnv1a_hash(std::string_view strv)
    {
        return fnv1a_hash(strv.length(), strv.data());
    }

    template<std::size_t N>
    constexpr hash_t fnv1a_hash(const char(&array)[N])
    {
        return fnv1a_hash(N - 1, &array[0]);
    }

    template<typename T>
    constexpr hash_t Type_Hash()
    {
        return fnv1a_hash(NAMEOF_TYPE(T));
    }

    template<typename T>
    constexpr hash_t Type_Hash(T t)
    {
        return Type_Hash<T>();
    }
}