#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"


namespace Warp::Gpu {

	class Device;

	class ShaderModule : public Inherit<ShaderModule, Object> {
	public:
		ShaderModule(Device* device, const std::vector<uint8_t>& spirv);

		~ShaderModule() override;

	private:
		VkShaderModule _shader;
		Device* _device {};
	};
}