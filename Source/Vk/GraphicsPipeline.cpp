#include "GraphicsPipeline.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp::Gpu;

GraphicsPipeline::GraphicsPipeline(Device* device, GraphicsPipelineState* state, RenderPass* renderpass, PipelineLayout* layout)
	: _device(device)
{
	VkViewport viewports[] = {
		{
			.x = 0.0f,
			.y = 0.0f,
			.width = 2.0f,
			.height = 2.0f,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		}
	};

	VkRect2D scissors[] = {
		{
			.offset = { 0, 0 },
			.extent = { 2, 2 }
		}
	};

	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = viewports,
		.scissorCount = 1,
		.pScissors = scissors
	};

	const VkGraphicsPipelineCreateInfo pipelineInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stageCount = to_u32(state->_shader_stage_cis.size()),
		.pStages = state->_shader_stage_cis.data(),
		.pVertexInputState = &state->_vertex_input_ci,
		.pInputAssemblyState = &state->_input_assembly_ci,
		.pTessellationState = &state->_tessellation_ci,
		.pViewportState = &viewportState,
		.pRasterizationState = &state->_rasterization_ci,
		.pMultisampleState = &state->_multisample_ci,
		.pDepthStencilState = &state->_depth_stencil_ci,
		.pColorBlendState = &state->_color_blend_ci,
		.pDynamicState = &state->_dynamic_ci,
		.layout = *layout,
		.renderPass = *renderpass,
		.subpass = 0
	};

	if(VkResult res = vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline); res != VK_SUCCESS) {
		MString msg = MString::format("Failed to create graphics pipeline: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(msg);
		throw Exception{ msg, res };
	}else {
		info("Created graphics pipeline.");
	}
}

GraphicsPipeline::~GraphicsPipeline() {
	if (_pipeline != VK_NULL_HANDLE){
		vkDestroyPipeline(*_device, _pipeline, nullptr);
	}
}
