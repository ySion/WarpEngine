#include "PipelineLayout.hpp"

#include "Device.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp::Gpu;

PipelineLayout::PipelineLayout(Device* device)
	: _device(device) {}


VkResult PipelineLayout::compile() {

	const VkPipelineLayoutCreateInfo ci{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = to_u32(_setLayouts.size()),
		.pSetLayouts = _setLayouts.data(),
		.pushConstantRangeCount = to_u32(_pushConstantRanges.size()),
		.pPushConstantRanges = _pushConstantRanges.data()
	};

	if (VkResult result = vkCreatePipelineLayout(*_device, &ci, nullptr, &_pipelineLayout); result != VK_SUCCESS) {
		MString msg = MString::format("Failed to create pipeline layout: {}, {}.", static_cast<int>(result), msg_map_VkResult(result));
		error(msg);
		return result;
	}

	return VK_SUCCESS;
}

PipelineLayout::~PipelineLayout() {
	if (_pipelineLayout != nullptr) {
		vkDestroyPipelineLayout(*_device, _pipelineLayout, nullptr);
	}
}
