#pragma once
#include "../Components/ComponentsCommon.h"

namespace thomas::transform
{
	DEFINE_TYPED_ID(transform_id);

	class component final
	{
	public:
		constexpr explicit component(transform_id id);
		constexpr component();

		constexpr transform_id get_id() const;
		constexpr bool is_valid() const;


		math::vector4f rotation() const;
		math::vector3f position() const;
		math::vector3f scale() const;

	private:
		transform_id m_id;
	};
}

