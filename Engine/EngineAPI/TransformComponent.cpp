#include "TransformComponent.h"

namespace thomas::transform
{
	constexpr component::component(transform_id id) : m_id(id)
	{
	}
	constexpr component::component() : m_id(id::invalid_id)
	{
	}
	constexpr transform_id component::get_id() const
	{
		return m_id;
	}
	constexpr bool component::is_valid() const
	{
		return id::is_valid(m_id);
	}
}