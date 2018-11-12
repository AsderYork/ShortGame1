#pragma once
#include <string_view>
namespace Helper
{
	/**
	Hashes a string literal into a numeric type.
	\tparam NumericType A numeric type, that should be returned.
	*/
	template<typename NumericType>
	class Hasher
	{
	public:
		constexpr static NumericType Hash(std::string_view Name, std::string_view CollisionSpace)
		{
			//Hashing algorithm djb2. Source:http://www.cse.yorku.ca/~oz/hash.html
			NumericType hash = 5381;

			for (auto letter : Name)
			{
				hash = ((hash << 5) + hash) + letter; /* hash * 33 + c */
			}
			return hash;
		}
	};
	
	template<>
	class Hasher<int32_t>
	{
	public:
		constexpr static int32_t Hash(std::string_view Name, std::string_view CollisionSpace)
		{
			//Hashing algorithm djb2. Source:http://www.cse.yorku.ca/~oz/hash.html
			int32_t hash = 5381;

			for (auto letter : Name)
			{
				hash = ((hash << 5) + hash) + letter; /* hash * 33 + c */
			}
			return hash;
		}
	};


}