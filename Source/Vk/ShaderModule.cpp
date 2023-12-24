#include "ShaderModule.hpp"

#include "Device.hpp"
#include "GlslCompiler.hpp"
#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp::Gpu;

ShaderModule::ShaderModule(Device* device, const std::vector<uint8_t>& spirv) : _device(device){
	const VkShaderModuleCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = to_u32(spirv.size() / sizeof(uint32_t)),
		.pCode = (uint32_t*)spirv.data()
	};

	VkResult res =  vkCreateShaderModule(*_device, &ci, nullptr, &_shader);
	if(res != VK_SUCCESS) {
		MString msg = MString::format("Failed to create shader module: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(msg);
		throw Exception{ msg, res };
	}
}

ShaderModule::~ShaderModule() {
	if(_shader) vkDestroyShaderModule(*_device, _shader, nullptr);
}