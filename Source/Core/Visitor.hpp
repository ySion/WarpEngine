#pragma once
#include "TypeName.hpp"
#include "Inherit.hpp"


WARP_TYPE_NAME(Visitor)

namespace Warp {

	class Visitor : public Inherit<Visitor, Object> {
	public:
		Visitor() = default;

		~Visitor() override = default;

		Visitor(const Visitor& other) = delete;

		Visitor(Visitor&& other) = delete;

		Visitor& operator=(const Visitor& other) = delete;

		Visitor& operator=(Visitor&& other) = delete;

		virtual void apply(Object* object);
	};
}
