#pragma once
#include <unordered_set>
#include <cassert>
namespace GEM::Helper
{
	/**!
	CRTP this class to guarantee, that every instance of the class will have unique value

	Derived - A type of class, that derives from this template
	UniqueValType - A type of unique identifier, that is used to identify instances.
	It it used to instanciate unordered_set so it must satisfy it's requirements.

	And remember - derived class is responsible for providing uniuqe identifiers, not this one
	*/
	template <typename Derived, typename UniqueValType>
	class UniqueInstanceHelper
	{
	protected:
		//Contains Unique value of all instances of derived class
		static std::unordered_set<UniqueValType> m_UnqiueProofSet;
		const UniqueValType m_UnqiueValue;
	public:
		UniqueInstanceHelper(const UniqueValType UnqiueValue) : m_UnqiueValue(UnqiueValue)
		{
			auto retval = m_UnqiueProofSet.insert(m_UnqiueValue);
			if (!retval.second)//If it wasn't written in;
			{
				throw std::exception("UniqueInstanceHelper - instances should have unique value, but this is not unique!");
			}
		}

		virtual ~UniqueInstanceHelper()
		{
			m_UnqiueProofSet.erase(m_UnqiueValue);
		}
		
	};
}