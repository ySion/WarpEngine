#pragma once

#include "VkBase.hpp"

#include "RenderPass.hpp"

#include "Core/Inherit.hpp"

namespace Warp::Gpu {


	class GraphicsPipelineState : public Inherit<GraphicsPipelineState, Object> {
	public:
		GraphicsPipelineState() = default;

		~GraphicsPipelineState() override = default;

		inline GraphicsPipelineState& add_shader_stage(
			VkShaderStageFlagBits stage,
			VkShaderModule shaderModule,
			const char* entryPointName = "main",
			const VkSpecializationInfo* specializationInfo = nullptr,
			VkPipelineShaderStageCreateFlags flags = 0)
		{
			VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
			pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo.stage = stage;
			pipelineShaderStageCreateInfo.module = shaderModule;
			pipelineShaderStageCreateInfo.pName = entryPointName;
			pipelineShaderStageCreateInfo.pSpecializationInfo = specializationInfo;
			pipelineShaderStageCreateInfo.flags = flags;
			_shader_stage_cis.emplace_back(pipelineShaderStageCreateInfo);
			return *this;
		}

		inline GraphicsPipelineState& set_vertex_input(
				const MVector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
				const MVector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions,
				VkPipelineVertexInputStateCreateFlags flags = 0) {
			_vertex_binding_descriptions = vertexBindingDescriptions;
			_vertex_attribute_descriptions = vertexAttributeDescriptions;

			_vertex_input_ci.vertexBindingDescriptionCount = static_cast<uint32_t>(_vertex_binding_descriptions.size());
			_vertex_input_ci.pVertexBindingDescriptions = _vertex_binding_descriptions.empty() ? nullptr : _vertex_binding_descriptions.data();
			_vertex_input_ci.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertex_attribute_descriptions.size());
			_vertex_input_ci.pVertexAttributeDescriptions = _vertex_attribute_descriptions.empty() ? nullptr : _vertex_attribute_descriptions.data();
			_vertex_input_ci.flags = flags;
			return *this;
		}

		inline GraphicsPipelineState& set_input_assembly(
			VkPrimitiveTopology topology,
			VkPipelineInputAssemblyStateCreateFlags flags = 0,
			VkBool32 primitiveRestartEnable = false) {
			_input_assembly_ci.topology = topology;
			_input_assembly_ci.flags = flags;
			_input_assembly_ci.primitiveRestartEnable = primitiveRestartEnable;
			return *this;
		}

		inline GraphicsPipelineState& set_tessellation(
			uint32_t patchControlPoints = 0,
			VkPipelineTessellationStateCreateFlags flags = 0) {
			_tessellation_ci.patchControlPoints = patchControlPoints;
			_tessellation_ci.flags = flags;
			return *this;
		}

		inline GraphicsPipelineState& set_rasterization(
			VkPolygonMode polygonMode,
			VkCullModeFlags cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
			VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
			VkPipelineRasterizationStateCreateFlags flags = 0) {
			_rasterization_ci.polygonMode = polygonMode;
			_rasterization_ci.cullMode = cullMode;
			_rasterization_ci.frontFace = frontFace;
			_rasterization_ci.flags = flags;
			_rasterization_ci.depthClampEnable = VK_FALSE;
			_rasterization_ci.lineWidth = 1.0f;
			return *this;
		}

		inline GraphicsPipelineState& set_multisample(
				VkSampleCountFlagBits rasterizationSamples,
				VkPipelineMultisampleStateCreateFlags flags = 0) {
			_multisample_ci.rasterizationSamples = rasterizationSamples;
			_multisample_ci.flags = flags;
			return *this;
		}

		inline GraphicsPipelineState& set_depth_stencil(
			VkBool32 depthTestEnable,
			VkBool32 depthWriteEnable,
			VkCompareOp depthCompareOp,
			VkPipelineDepthStencilStateCreateFlags flags = 0) {
			_depth_stencil_ci.depthTestEnable = depthTestEnable;
			_depth_stencil_ci.depthWriteEnable = depthWriteEnable;
			_depth_stencil_ci.depthCompareOp = depthCompareOp;
			_depth_stencil_ci.flags = flags;
			return *this;
		}

		inline GraphicsPipelineState& add_color_blend_attachment(
			VkBool32 blendEnable = true,
			VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
			VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
			VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
			VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		) {
			_color_blend_attachments.emplace_back(VkPipelineColorBlendAttachmentState{
				.blendEnable = blendEnable,
				.srcColorBlendFactor = srcColorBlendFactor,
				.dstColorBlendFactor = dstColorBlendFactor,
				.colorBlendOp = colorBlendOp,
				.srcAlphaBlendFactor = srcAlphaBlendFactor,
				.dstAlphaBlendFactor = dstAlphaBlendFactor,
				.alphaBlendOp = alphaBlendOp,
				.colorWriteMask = colorWriteMask
			});
			_color_blend_ci.attachmentCount = static_cast<uint32_t>(_color_blend_attachments.size());
			_color_blend_ci.pAttachments = _color_blend_attachments.data();

			return *this;
		}

		inline GraphicsPipelineState& add_dynamic_state(VkDynamicState dynamicState) {
			_dynamic_states.emplace_back(dynamicState);
			_dynamic_ci.pDynamicStates = _dynamic_states.data();
			_dynamic_ci.dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size());
			return *this;
		}

		MVector<VkPipelineShaderStageCreateInfo> _shader_stage_cis{};
		VkPipelineVertexInputStateCreateInfo _vertex_input_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		VkPipelineInputAssemblyStateCreateInfo _input_assembly_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		VkPipelineTessellationStateCreateInfo _tessellation_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
		VkPipelineRasterizationStateCreateInfo _rasterization_ci{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		VkPipelineMultisampleStateCreateInfo _multisample_ci{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		VkPipelineDepthStencilStateCreateInfo _depth_stencil_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		VkPipelineColorBlendStateCreateInfo _color_blend_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		VkPipelineDynamicStateCreateInfo _dynamic_ci{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	private:
		MVector<VkVertexInputBindingDescription> _vertex_binding_descriptions{};
		MVector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions{};
		MVector<VkPipelineColorBlendAttachmentState> _color_blend_attachments{};
		MVector<VkDynamicState> _dynamic_states{};
	};
}