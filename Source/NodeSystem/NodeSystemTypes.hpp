#pragma once
#include <stdint.h>

namespace Warp::NodeSystem {

	enum class PinType : uint8_t {
		Single,  // one connect only
		Thin,    // multi connect only but shared
		Width,   // multi connect only but shared, and width
	};

	enum class ContextType : uint8_t {
		Porcedural,
		PorceduralCode,
		PorceduralVop,
		Logic,
		LogicCode,
		LogicVop,
		Program,
		ProgramCode,
		ProgramVop,
	};
};