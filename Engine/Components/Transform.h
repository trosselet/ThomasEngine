#pragma once
#include "ComponentsCommon.h"

namespace thomas::transform
{
	DEFINE_TYPED_ID(transform_id);

	struct init_info
	{
		float32 position[3]{};
		float32 rotation[4]{};
		float32 scale[3]{1.f, 1.f, 1.f};
	};

	transform_id create_transform(const init_info& info, game_entity::entity_id entity_id);
	void remove_transform(transform_id);
}