#pragma once

#include <tuple>

namespace GEM::Helper
{
	namespace _detail_
	{
		template<int index, typename Callback, typename... Args>
		struct iterate_tuple
		{
			static void next(std::tuple<Args...>& t, Callback callback)
			{
				// ��������� ������� � ���������� �������� ���� �� ����� 
				iterate_tuple<index - 1, Callback, Args...>::next(t, callback);

				// �������� ���������� � �������� ��� ������� � �������� ��������
				callback(index, std::get<index>(t));
			}
		};

		// ��������� ������������� ��� ������� 0 (��������� ��������)
		template<typename Callback, typename... Args>
		struct iterate_tuple<0, Callback, Args...>
		{
			static void next(std::tuple<Args...>& t, Callback callback)
			{
				callback(0, std::get<0>(t));
			}
		};

		// ��������� ������������� ��� ������� -1 (������ ������)
		template<typename Callback, typename... Args>
		struct iterate_tuple<-1, Callback, Args...>
		{
			static void next(std::tuple<Args...>& t, Callback callback)
			{
				// ������ �� ������
			}
		};
	}

	template<typename Callback, typename... Args>
	void for_each(std::tuple<Args...>& t, Callback callback)
	{
		// ������ �������
		int const t_size = std::tuple_size<std::tuple<Args...>>::value;

		// ��������� ����������� ����� ��������� ������� �� ����� ����������
		_detail_::iterate_tuple<t_size - 1, Callback, Args...>::next(t, callback);
	}

}