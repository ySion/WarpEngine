#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, ShaderModule);

namespace Warp::Gpu {

	class Device;

	class ShaderModule : public Inherit<ShaderModule, Object> {
	public:
		ShaderModule(Device* device, const MVector<uint32_t>& spirv);

		~ShaderModule() override;

		inline Device* get_device() const { return _device; }

		inline VkShaderModule vk() const { return _shader; }

		operator VkShaderModule() const { return _shader; }

	private:
		VkShaderModule _shader;
		Device* _device {};
	};
}