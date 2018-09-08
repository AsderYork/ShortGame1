#pragma once
#include <cereal\cereal.hpp>
#include <limits>
#include "LogHelper.h"

namespace Helper
{
	template <class SizeTagType, class Archive, class T, class A> inline
		typename std::enable_if<cereal::traits::is_output_serializable<cereal::BinaryData<T>, Archive>::value
		&& std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
		SaveVector(Archive & ar, std::vector<T, A> const & vector)
	{
		if (vector.size() > (std::numeric_limits<SizeTagType>::max)())
		{
			LOGCATEGORY("VariableSizeVectorSerialization").error("Vector size is %i, it cannot be represented by given type with max size %i", vector.size(), (std::numeric_limits<SizeTagType>::max)());
		}
		ar(cereal::make_size_tag(static_cast<SizeTagType>(vector.size()))); // number of elements
		ar(cereal::binary_data(vector.data(), vector.size() * sizeof(T)));
	}

	template <class SizeTagType, class Archive, class T, class A> inline
		typename std::enable_if<cereal::traits::is_input_serializable<cereal::BinaryData<T>, Archive>::value
		&& std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
		LoadVector(Archive & ar, std::vector<T, A> & vector)
	{
		SizeTagType vectorSize;
		ar(cereal::make_size_tag(vectorSize));

		vector.resize(static_cast<std::size_t>(vectorSize));
		ar(cereal::binary_data(vector.data(), static_cast<std::size_t>(vectorSize) * sizeof(T)));
	}


	//! Serialization for non-arithmetic vector types
	template <class SizeTagType, class Archive, class T, class A> inline
		typename std::enable_if<!cereal::traits::is_output_serializable<cereal::BinaryData<T>, Archive>::value
		|| !std::is_arithmetic<T>::value, void>::type
		SaveVector(Archive & ar, std::vector<T, A> const & vector)
	{
		if (vector.size() > (std::numeric_limits<SizeTagType>::max)())
		{
			LOGCATEGORY("VariableSizeVectorSerialization").error("Vector size is %i, it cannot be represented by given type with max size %i", vector.size(), (std::numeric_limits<SizeTagType>::max)());
		}
		ar(cereal::make_size_tag(static_cast<SizeTagType>(vector.size()))); // number of elements
		for (auto && v : vector)
			ar(v);
	}

	//! Serialization for non-arithmetic vector types
	template <class SizeTagType, class Archive, class T, class A> inline
		typename std::enable_if<!cereal::traits::is_input_serializable<cereal::BinaryData<T>, Archive>::value
		|| !std::is_arithmetic<T>::value, void>::type
		LoadVector(Archive & ar, std::vector<T, A> & vector)
	{
		SizeTagType size;
		ar(cereal::make_size_tag(size));

		vector.resize(static_cast<std::size_t>(size));
		for (auto && v : vector)
			ar(v);
	}
}