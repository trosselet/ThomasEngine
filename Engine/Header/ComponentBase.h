#ifndef COMPONENT_BASE_INCLUDE__H
#define COMPONENT_BASE_INCLUDE__H

#include <Tools/Header/PrimitiveTypes.h>
#include <Engine/Header/Component.h>

template<uint16 T>
struct ComponentBase : Component
{
	static inline uint16 const Tag = T;
};

#endif // !COMPONENT_BASE_INCLUDE__H
