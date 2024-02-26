#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, ShaderModule);

namespace Warp::Gpu {

	class Device;

	enum class ShaderType : uint8_t {
		Vertex,
		Tesscontrol,
		Tessevaluation,
		Geometry,
		Fragment,
		Compute,
		Raygen,
		Intersect,
		Anyhit,
		Closesthit,
		Miss,
		Callable,
		Task,
		Mesh,
		None,
	};

	std::string_view shader_type_to_string(ShaderType type);

	class ShaderModule : public Inherit<ShaderModule, Object> {
	public:
		ShaderModule(Device* device, ShaderType type, const MVector<uint32_t>& spirv);

		~ShaderModule() override;

		inline Device* get_device() const { return _device; }

		inline VkShaderModule vk() const { return _shader; }

		inline ShaderType type() const { return _type;}

		operator VkShaderModule() const { return _shader; }

	private:
		ShaderType _type;
		VkShaderModule _shader;
		Device* _device {};
	};
}