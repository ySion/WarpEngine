#pragma once

#include "Core/Inherit.hpp"
WARP_TYPE_NAME_2(Gpu, MaterialShader);

namespace Warp::Gpu {

	class MaterialShader : public Inherit<MaterialShader, Object> {
	public:
		MaterialShader() = default;
		~MaterialShader() override = default;

		void parse_from_source(const MString& source)
		{


		}
	private:

	};

}