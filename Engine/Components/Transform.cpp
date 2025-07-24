#include "Transform.h"

namespace thomas::transform
{
	namespace
	{
		utilities::vector< math::vector4f > rotations;
		utilities::vector< math::vector3f > positions;
		utilities::vector< math::vector3f > scales;
	}

	component create_transform(const init_info& info, game_entity::entity entity)
	{
		assert(entity.is_valid());
		const id::id_type entity_index(id::index(entity.get_id()));

		if (positions.size() > entity_index)
		{
			rotations[entity_index] = math::vector4f(info.rotation);
			positions[entity_index] = math::vector3f(info.position);
			scales[entity_index] = math::vector3f(info.scale);
		}
		else
		{
			assert(positions.size() == entity_index);
			rotations.emplace_back(info.rotation);
			positions.emplace_back(info.position);
			scales.emplace_back(info.scale);
		}

		return component(transform_id((id::id_type)positions.size() - 1));
	}

	void remove_transform(component c)
	{
		assert(c.is_valid());
	}

	math::vector4f component::rotation() const
	{
		assert(is_valid());
		return rotations[id::index(m_id)];
	}

	math::vector3f component::position() const
	{
		assert(is_valid());
		return positions[id::index(m_id)];
	}

	math::vector3f component::scale() const
	{
		assert(is_valid());
		return scales[id::index(m_id)];
	}
}