#pragma once
#include "ComponentsCommon.h"

namespace thomas::transform
{
	struct init_info
	{
		float32 position[3]{};
		float32 rotation[4]{};
		float32 scale[3]{1.f, 1.f, 1.f};
	};

	component create_transform(const init_info& info, game_entity::entity entity);
	void remove_transform(component c);
}