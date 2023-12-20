#pragma once

#include "VkBase.hpp"

#include "RenderPass.hpp"

#include "Core/Inherit.hpp"

namespace Warp::Gpu {

	//Helpers

	inline VkPipelineShaderStageCreateInfo ci_PipelineShaderStage(
			VkShaderStageFlagBits stage,
			VkShaderModule shaderModule,
			const char* entryPointName = "main",
			const VkSpecializationInfo* specializationInfo = nullptr,
			VkPipelineShaderStageCreateFlags flags = 0) {
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.stage = stage;
		pipelineShaderStageCreateInfo.module = shaderModule;
		pipelineShaderStageCreateInfo.pName = entryPointName;
		pipelineShaderStageCreateInfo.pSpecializationInfo = specializationInfo;
		pipelineShaderStageCreateInfo.flags = flags;
		return pipelineShaderStageCreateInfo;
	}

	inline VkPipelineInputAssemblyStateCreateInfo ci_PipelineInputAssemblyState(
			VkPrimitiveTopology topology,
			VkPipelineInputAssemblyStateCreateFlags flags = 0,
			VkBool32 primitiveRestartEnable = false) {
		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.topology = topology;
		pipelineInputAssemblyStateCreateInfo.flags = flags;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
		return pipelineInputAssemblyStateCreateInfo;
	}

	inline VkPipelineVertexInputStateCreateInfo ci_PipelineVertexInputState(
			const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions,
			VkPipelineVertexInputStateCreateFlags flags = 0) {
		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
		pipelineVertexInputStateCreateInfo.flags = flags;
		return pipelineVertexInputStateCreateInfo;
	}

	inline VkPipelineTessellationStateCreateInfo ci_PipelineTessellationState(
			uint32_t patchControlPoints = 0,
			VkPipelineTessellationStateCreateFlags flags = 0) {
		VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
		pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
		pipelineTessellationStateCreateInfo.flags = flags;
		return pipelineTessellationStateCreateInfo;
	}

	inline VkPipelineRasterizationStateCreateInfo ci_PipelineRasterizationState(
			VkPolygonMode polygonMode,
			VkCullModeFlags cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
			VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
			VkPipelineRasterizationStateCreateFlags flags = 0) {
		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
		pipelineRasterizationStateCreateInfo.cullMode = cullMode;
		pipelineRasterizationStateCreateInfo.frontFace = frontFace;
		pipelineRasterizationStateCreateInfo.flags = flags;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		return pipelineRasterizationStateCreateInfo;
	}

	inline VkPipelineMultisampleStateCreateInfo ci_PipelineMultisampleState(
			VkSampleCountFlagBits rasterizationSamples,
			VkPipelineMultisampleStateCreateFlags flags = 0) {
		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
		pipelineMultisampleStateCreateInfo.flags = flags;
		return pipelineMultisampleStateCreateInfo;
	}

	inline VkPipelineDepthStencilStateCreateInfo ci_PipelineDepthStencilState(
			VkBool32 depthTestEnable,
			VkBool32 depthWriteEnable,
			VkCompareOp depthCompareOp,
			VkPipelineDepthStencilStateCreateFlags flags = 0) {
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
		pipelineDepthStencilStateCreateInfo.flags = flags;
		return pipelineDepthStencilStateCreateInfo;
	}

	inline VkPipelineColorBlendAttachmentState ci_PipelineColorBlendAttachmentState(
			VkBool32 blendEnable = true,
			VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
			VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
			VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
			VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	) {
		return VkPipelineColorBlendAttachmentState {
			.blendEnable = blendEnable,
			.srcColorBlendFactor = srcColorBlendFactor,
			.dstColorBlendFactor = dstColorBlendFactor,
			.colorBlendOp = colorBlendOp,
			.srcAlphaBlendFactor = srcAlphaBlendFactor,
			.dstAlphaBlendFactor = dstAlphaBlendFactor,
			.alphaBlendOp = alphaBlendOp,
			.colorWriteMask = colorWriteMask
		};
	}

	inline VkPipelineColorBlendStateCreateInfo ci_PipelineColorBlendState(
			const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
			VkPipelineColorBlendStateCreateFlags flags = 0) {
		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		pipelineColorBlendStateCreateInfo.pAttachments = attachments.data();
		pipelineColorBlendStateCreateInfo.flags = flags;
		return pipelineColorBlendStateCreateInfo;
	}

	inline VkPipelineDynamicStateCreateInfo ci_PipelineDynamicState(
			const std::vector<VkDynamicState>& dynamicStates,
			VkPipelineDynamicStateCreateFlags flags = 0) {
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
		pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		pipelineDynamicStateCreateInfo.flags = flags;
		return pipelineDynamicStateCreateInfo;
	}

	class GraphicsPipelineState : public Inherit<GraphicsPipelineState, Object> {
	public:
		GraphicsPipelineState() = default;

		~GraphicsPipelineState() override = default;

		std::vector<VkPipelineShaderStageCreateInfo> _shader_stage_cis {};
		VkPipelineVertexInputStateCreateInfo _vertex_input_ci {} ;
		VkPipelineInputAssemblyStateCreateInfo _input_assembly_ci {};
		VkPipelineTessellationStateCreateInfo _tessellation_ci {};
		VkPipelineRasterizationStateCreateInfo _rasterization_ci {};
		VkPipelineMultisampleStateCreateInfo _multisample_ci {};
		VkPipelineDepthStencilStateCreateInfo _depth_stencil_ci {};
		VkPipelineColorBlendStateCreateInfo _color_blend_ci {};
		VkPipelineDynamicStateCreateInfo _dynamic_ci {};
		RenderPass* _render_pass;
	};
}