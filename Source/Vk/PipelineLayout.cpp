#include "PipelineLayout.hpp"

#include "Device.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"
using namespace Warp::Gpu;

PipelineLayout::PipelineLayout(Device* device, 
	uint32_t setLayoutCount,
	const VkDescriptorSetLayout* pSetLayouts,
	uint32_t pushConstantRangeCount,
	const VkPushConstantRange* pPushConstantRanges)
: _device(device)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
	pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
	pipelineLayoutCreateInfo.pPushConstantRanges = pPushConstantRanges;

	if (VkResult result = vkCreatePipelineLayout(*_device, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout); result != VK_SUCCESS) {
		MString msg = MString::format("Failed to create pipeline layout: {}", static_cast<int>(result));
		error(msg);
		throw Exception{msg, result};
	}
}

PipelineLayout::~PipelineLayout() {
	if(_pipelineLayout != nullptr) {
		vkDestroyPipelineLayout(*_device, _pipelineLayout, nullptr);
	}
}
