#pragma once

#include "../Components/ComponentsCommon.h"
#include "TransformComponent.h"

namespace thomas::game_entity
{
	DEFINE_TYPED_ID(entity_id);

	class entity
	{
	public:
		constexpr explicit entity(entity_id id);
		constexpr entity();

		constexpr entity_id get_id() const;
		constexpr bool is_valid() const;

		transform::component transform() const;

	private:
		entity_id m_id;
	};
}