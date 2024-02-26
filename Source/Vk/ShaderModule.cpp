#include "ShaderModule.hpp"

#include "Device.hpp"
#include "GlslCompiler.hpp"
#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp::Gpu;

std::string_view Warp::Gpu::shader_type_to_string(ShaderType type)
{

	switch (type) {
	case ShaderType::Vertex:
		return "Vertex";
	case ShaderType::Tesscontrol:
		return "Tesscontrol";
	case ShaderType::Tessevaluation:
		return "Tessevaluation";
	case ShaderType::Geometry:
		return "Geometry";
	case ShaderType::Fragment:
		return "Fragment";
	case ShaderType::Compute:
		return "Compute";
	case ShaderType::Raygen:
		return "Raygen";
	case ShaderType::Intersect:
		return "Intersect";
	case ShaderType::Anyhit:
		return "Anyhit";
	case ShaderType::Closesthit:
		return "Closesthit";
	case ShaderType::Miss:
		return "Miss";
	case ShaderType::Callable:
		return "Callable";
	case ShaderType::Task:
		return "Task";
	case ShaderType::Mesh:
		return "Mesh";
	case ShaderType::None:
		return "None";
	default:
		return "Unknown";
	}
}

ShaderModule::ShaderModule(Device* device, ShaderType type, const MVector<uint32_t>& spirv) : _type(type),  _device(device){
	const VkShaderModuleCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = to_u32(spirv.size() * sizeof(uint32_t)),
		.pCode = spirv.data()
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