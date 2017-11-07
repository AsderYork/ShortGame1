#pragma once
#include <functional>
#include <tuple>
#include <array>
#include <type_traits>


template<int>
struct InteratablePlaceholder {
};

namespace std {
	template<int N>
	struct is_placeholder<InteratablePlaceholder<N>> : public integral_constant<int, N + 1> {};
}


template<typename T>
T ParamToStr(std::string val)
{}
template<>
inline int ParamToStr(std::string val)
{
	return std::stoi(val);
}
template<>
inline bool ParamToStr(std::string val)
{
	return val == "1" ? true : false;
}
template<>
inline float ParamToStr(std::string val)
{
	return std::stof(val);
}
template<>
inline std::string ParamToStr(std::string val)
{
	return val;
}


template<typename T>
std::string ResultToStr(T val) { return std::to_string(val); }

//Non-static class functions
template<class ParamClass, typename T, std::size_t...I, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, ParamClass* Obj, T(ParamClass::*Func)(Args...))
{
	auto StatFunc = std::bind(Func, Obj, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		return ResultToStr(std::bind(StatFunc, Params[I]...)());
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};

template<class ParamClass, std::size_t...I, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, ParamClass* Obj, void(ParamClass::*Func)(Args...))
{
	auto StatFunc = std::bind(Func, Obj, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		std::bind(StatFunc, Params[I]...)();
		return std::string();
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};

//Free functions
template<std::size_t...I, typename T, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, T(*Func)(Args...))
{
	auto StatFunc = std::bind(Func, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		return ResultToStr(std::bind(StatFunc, Params[I]...)());
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};

template<std::size_t...I, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, void(*Func)(Args...))
{
	auto StatFunc = std::bind(Func, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		std::bind(StatFunc, Params[I]...)();
		return std::string();
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};

//Functional objects
template<std::size_t...I, typename T, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, std::function<T(Args...)> Func)
{
	auto StatFunc = std::bind(Func, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		return ResultToStr(std::bind(StatFunc, Params[I]...)());
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};

template<std::size_t...I, typename... Args>
decltype(auto) FomralizeFunctionImpl(std::index_sequence<I...>, std::function<void(Args...)> Func)
{
	auto StatFunc = std::bind(Func, std::bind(ParamToStr<Args>, InteratablePlaceholder<I>())...);


	auto Lambda = [StatFunc](std::vector<std::string>& Params) {
		std::bind(StatFunc, Params[I]...)();
		return std::string();
	};
	return std::function<std::string(std::vector<std::string>&)>(Lambda);
};




template<class ParamClass, typename T, typename... Args>
decltype(auto) FomralizeFunction(ParamClass* Obj, T(ParamClass::*Func)(Args...))
{
	using Indices = std::make_index_sequence<sizeof...(Args)>;
	return std::make_pair(FomralizeFunctionImpl(Indices{}, Obj, Func), sizeof...(Args));


};

template<typename T, typename... Args>
decltype(auto) FomralizeFunction(T(*Func)(Args...))
{
	using Indices = std::make_index_sequence<sizeof...(Args)>;
	return std::make_pair(FomralizeFunctionImpl(Indices{}, Func), sizeof...(Args));

};

template<typename T, typename... Args>
decltype(auto) FomralizeFunction(std::function<T(Args...)> Func)
{
	using Indices = std::make_index_sequence<sizeof...(Args)>;
	return std::make_pair(FomralizeFunctionImpl(Indices{}, Func), sizeof...(Args));

};