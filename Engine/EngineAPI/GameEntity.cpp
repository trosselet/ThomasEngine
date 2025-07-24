#include "GameEntity.h"

namespace thomas::game_entity
{
	constexpr entity::entity(entity_id id) : m_id(id)
	{
	}

	constexpr entity::entity() : m_id(id::invalid_id)
	{
	}

	constexpr entity_id entity::get_id() const
	{
		return m_id;
	}
	constexpr bool entity::is_valid() const
	{
		return id::is_valid(m_id);
	}
}
