#pragma once

#include "Graphics/GPUResource.h"

namespace Warp::Render
{

	struct MaterialParameter
	{
		
	};

	struct MaterialSource {
		GPU::GPUShader* shader;
		MaterialParameter* parameters;

	};

	struct MaterialInstance {

	};
}