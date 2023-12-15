#pragma once

#include "Inherit.hpp"
#include "Object.hpp"

namespace Warp {

	WARP_TYPE_NAME(Modifier);
	
	class Modifier : public Inherit<Modifier, Object> {
	public:
		Modifier() = default;

		~Modifier() override = default;

		Modifier(const Modifier& other) = delete;

		Modifier(Modifier&& other) = delete;

		Modifier& operator=(const Modifier& other) = delete;

		Modifier& operator=(Modifier&& other) = delete;

		virtual void apply(Object* object);
	};

	
}
