#pragma once
#include "Mixin_Base.h"

namespace GEM::GameSim
{
	using GAMEOBJECT_TYPE_ID = int32_t;
	constexpr GAMEOBJECT_TYPE_ID UNKNOWN_OBJECT_TYPE = 0;

	class Mixin_Objecttype : public Mixin_base
	{
	private:

		GAMEOBJECT_TYPE_ID m_typeid = UNKNOWN_OBJECT_TYPE;


	public:
		MIXIN_ID(1)

		GAMEOBJECT_TYPE_ID inline gettypeid() const { return m_typeid; }
		void inline settypeid(GAMEOBJECT_TYPE_ID id) { m_typeid = id; }

		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual bool CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag) override;

		virtual void ApplyUpdate(cereal::BinaryInputArchive& archive) override;

		virtual bool tick(const GameTime delta) override {
			return true;
		}

	};

}