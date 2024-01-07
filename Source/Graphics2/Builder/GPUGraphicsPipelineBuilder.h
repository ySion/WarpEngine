#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUGraphicsPipeline.h"
#include "Graphics/GPUResource/GPUShader.h"
#include "Graphics/GPUResource/GPURenderPass.h"
#include "Graphics/GPUResource/GPUPipelineLayout.h"

namespace Warp {

	namespace GPU {
		template<>
		class GPUResourceBuilder<GPUGraphicsPipeline> {
		public:
			using target_type = GPUGraphicsPipeline;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkGraphicsPipelineCreateInfo ci_pipeline{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
				};

				//Shader
				GPUShader* shader_vertex{};
				GPUShader* shader_tess_control{};
				GPUShader* shader_tess_evaluation{};
				GPUShader* shader_geometry{};
				GPUShader* shader_fragment{};

				MVector<VkPipelineShaderStageCreateInfo> ci_shader_stage{ };

				//Vertex Input
				MVector<VkVertexInputBindingDescription> vertex_input_binding_descriptions{};
				MVector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions{};

				//Input Assembly
				VkPipelineInputAssemblyStateCreateInfo ci_input_assembly{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				};

				//Tessellation
				VkPipelineTessellationStateCreateInfo ci_tessellation{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
				};

				//Viewport
				MVector<VkRect2D> scissors {};
				MVector<VkViewport> view_ports {};
				VkPipelineViewportStateCreateInfo ci_viewport{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,};

				//Rasterization
				VkPipelineRasterizationStateCreateInfo ci_rasterization{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.polygonMode = VK_POLYGON_MODE_FILL,
					.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
					.frontFace = VK_FRONT_FACE_CLOCKWISE,
					.lineWidth = 1.0f,
				};

				//MultiSample
				VkPipelineMultisampleStateCreateInfo ci_multisample{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				};

				//DepthStencil
				VkPipelineDepthStencilStateCreateInfo ci_depth_stencil{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthCompareOp = VK_COMPARE_OP_LESS,
				};

				MVector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states{};
				VkPipelineColorBlendStateCreateInfo ci_color_blend{};

				//Dynamic state
				MVector<VkDynamicState> dynamics{ VK_DYNAMIC_STATE_VIEWPORT , VK_DYNAMIC_STATE_SCISSOR };
				VkPipelineDynamicStateCreateInfo ci_dynamic{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				};

				GPUPipelineLayout* layout{};
				GPURenderPass* render_pass{};
				uint32_t subpass_idx = 0;
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
				set_viewport_count(1);
				set_dynamic_state({});
			}

			self_type& set_state_shaders(
					GPUShader* vertex = nullptr,
					GPUShader* tess_control = nullptr,
					GPUShader* tess_evaluation = nullptr,
					GPUShader* geometry = nullptr,
					GPUShader* fragment = nullptr) {

				create_info.ci_shader_stage.clear();

				if (vertex) {
					const VkPipelineShaderStageCreateInfo stage_create_info{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = vertex->m_module,
						.pName = "main",
						.pSpecializationInfo = nullptr
					};
					create_info.ci_shader_stage.push_back(stage_create_info);
				}

				if (tess_control) {
					const VkPipelineShaderStageCreateInfo stage_create_info{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
						.module = tess_control->m_module,
						.pName = "main",
						.pSpecializationInfo = nullptr
					};
					create_info.ci_shader_stage.push_back(stage_create_info);
				}

				if (tess_evaluation) {
					const VkPipelineShaderStageCreateInfo stage_create_info{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
						.module = tess_evaluation->m_module,
						.pName = "main",
						.pSpecializationInfo = nullptr
					};
					create_info.ci_shader_stage.push_back(stage_create_info);
				}

				if (geometry) {
					const VkPipelineShaderStageCreateInfo stage_create_info{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_GEOMETRY_BIT,
						.module = geometry->m_module,
						.pName = "main",
						.pSpecializationInfo = nullptr
					};
					create_info.ci_shader_stage.push_back(stage_create_info);
				}

				if (fragment) {
					const VkPipelineShaderStageCreateInfo stage_create_info{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = fragment->m_module,
						.pName = "main",
						.pSpecializationInfo = nullptr
					};
					create_info.ci_shader_stage.push_back(stage_create_info);
				}

				create_info.shader_vertex = vertex;
				create_info.shader_tess_control = tess_control;
				create_info.shader_tess_evaluation = tess_evaluation;
				create_info.shader_geometry = geometry;
				create_info.shader_fragment = fragment;

				create_info.ci_pipeline.stageCount = static_cast<uint32_t>(create_info.ci_shader_stage.size());
				create_info.ci_pipeline.pStages = create_info.ci_shader_stage.data();
				return *this;
			}

			self_type& set_state_input_assembly(VkPrimitiveTopology topology, bool primitive_restart_enable = false) {

				create_info.ci_input_assembly.topology = topology;
				create_info.ci_input_assembly.primitiveRestartEnable = primitive_restart_enable;

				return *this;
			}

			self_type& set_viewport_count(uint32_t count) {
				create_info.scissors.clear();
				create_info.view_ports.clear();

				for (int i = 0; i < count; ++i) {
					create_info.scissors.push_back({ .offset = {0,0}, .extent = {256, 256} });
					create_info.view_ports.push_back({ .x = 0, .y = 0, .width = 256 , .height = 256, .minDepth = 0.0f, .maxDepth = 1.0f });
				}

				create_info.ci_viewport = VkPipelineViewportStateCreateInfo {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.viewportCount = static_cast<uint32_t>(create_info.view_ports.size()),
					.pViewports = create_info.view_ports.data(),
					.scissorCount = static_cast<uint32_t>(create_info.scissors.size()),
					.pScissors = create_info.scissors.data()
				};

				return *this;
			}

			self_type& set_depth_state(bool depth_test,
				bool depth_write = true,
				VkCompareOp pixel_new_op_old = VK_COMPARE_OP_LESS,
				bool depth_bounds_test_enable = false,
				float min_depth_bounds = 0.0f,
				float max_depth_bounds = 1.0f
			) {
				create_info.ci_depth_stencil.depthTestEnable = depth_test;
				create_info.ci_depth_stencil.depthWriteEnable = depth_write;
				create_info.ci_depth_stencil.depthCompareOp = pixel_new_op_old;
				create_info.ci_depth_stencil.depthBoundsTestEnable = depth_bounds_test_enable;
				create_info.ci_depth_stencil.minDepthBounds = min_depth_bounds;
				create_info.ci_depth_stencil.maxDepthBounds = max_depth_bounds;

				return *this;
			}

			constexpr self_type& set_stencil_state(bool stencil_test, const VkStencilOpState& front_op, const VkStencilOpState& back_op) {
				create_info.ci_depth_stencil.stencilTestEnable = stencil_test;
				create_info.ci_depth_stencil.front = front_op;
				create_info.ci_depth_stencil.back = back_op;
				return *this;
			}

			self_type& set_state_rasterization(
				VkPolygonMode polygon_mode,
				VkCullModeFlags cull_mode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
				VkFrontFace front_face = VK_FRONT_FACE_CLOCKWISE,
				bool depth_clamp_enable = false,
				bool depth_bas_enable = false,
				float depth_bias_constant_factor = 0.0f,
				float depth_bias_clamp = 0.0f,
				float depth_bias_slope_factor = 0.0f
			) {
				create_info.ci_rasterization = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.depthClampEnable = depth_clamp_enable,
					.rasterizerDiscardEnable = false,
					.polygonMode = polygon_mode,
					.cullMode = cull_mode,
					.frontFace = front_face,
					.depthBiasEnable = depth_bas_enable,
					.depthBiasConstantFactor = depth_bias_constant_factor,
					.depthBiasClamp = depth_bias_clamp,
					.depthBiasSlopeFactor = depth_bias_slope_factor,
					.lineWidth = 1.0f
				};
				return *this;
			}

			self_type& set_state_multisample(VkSampleCountFlagBits sample_count, bool sample_shading_enable = false, float min_sample_shading = 1.0f) {
				create_info.ci_multisample = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.rasterizationSamples = sample_count,
					.sampleShadingEnable = sample_shading_enable,
					.minSampleShading = min_sample_shading,
				};
				return *this;
			}

			constexpr self_type& set_state_color_blend(bool enable_logic_op = false,
				VkLogicOp logic_op = VK_LOGIC_OP_COPY, const std::array<float, 4>& blend_constans = { 0,0,0,0 }
			) {

				create_info.ci_color_blend = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.logicOpEnable = enable_logic_op,
					.logicOp = logic_op,
					.attachmentCount = (uint32_t)create_info.color_blend_attachment_states.size(),
					.pAttachments = create_info.color_blend_attachment_states.data(),
				};
				create_info.ci_color_blend.blendConstants[0] = blend_constans[0];
				create_info.ci_color_blend.blendConstants[1] = blend_constans[1];
				create_info.ci_color_blend.blendConstants[2] = blend_constans[2];
				create_info.ci_color_blend.blendConstants[3] = blend_constans[3];
				return *this;
			}

			self_type& set_layout(GPUPipelineLayout* layout) {
				create_info.layout = layout;
				create_info.ci_pipeline.layout = layout->m_layout;
				return *this;
			}

			self_type& set_render_pass(GPURenderPass* render_pass, uint32_t subpass_idx) {
				create_info.render_pass = render_pass;
				create_info.subpass_idx = subpass_idx;
				create_info.ci_pipeline.renderPass = render_pass->m_render_pass;
				create_info.ci_pipeline.subpass = subpass_idx;
				return *this;
			}

			self_type& add_color_blend_attachment(bool blend_enable = false,
				VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_SRC_ALPHA,
				VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VkBlendOp color_blend_op = VK_BLEND_OP_ADD,
				VkBlendFactor src_alpha_blend = VK_BLEND_FACTOR_SRC_ALPHA,
				VkBlendFactor dst_alpha_blend = VK_BLEND_FACTOR_DST_ALPHA,
				VkBlendOp alpha_blend_op = VK_BLEND_OP_ADD,
				VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			) {
				create_info.color_blend_attachment_states.push_back({
						.blendEnable = blend_enable,
						.srcColorBlendFactor = src_color_blend_factor,
						.dstColorBlendFactor = dst_color_blend_factor,
						.colorBlendOp = color_blend_op,
						.srcAlphaBlendFactor = src_alpha_blend,
						.dstAlphaBlendFactor = dst_alpha_blend,
						.alphaBlendOp = alpha_blend_op,
						.colorWriteMask = color_write_mask
				});
				return *this;
			}

			self_type& add_vertex_buffer(const VkVertexInputBindingDescription& binding_desc, const MVector<VkVertexInputAttributeDescription>& attributes) {

				create_info.vertex_input_binding_descriptions.push_back(binding_desc);
				create_info.vertex_input_attribute_descriptions.insert(create_info.vertex_input_attribute_descriptions.end(),
					attributes.begin(), attributes.end());

				return *this;
			}

			//自动开启VK_DYNAMIC_STATE_VIEWPORT和VK_DYNAMIC_STATE_SCISSOR
			self_type& set_dynamic_state(const MVector<VkDynamicState>& states) {

				create_info.dynamics = states;

				create_info.dynamics.push_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);
				create_info.dynamics.push_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

				return *this;
			}

			GPUGraphicsPipeline* make(bool replace = true) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				const VkPipelineVertexInputStateCreateInfo ci_vertex_input = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.pNext = nullptr ,
					.flags = 0,
					.vertexBindingDescriptionCount = static_cast<uint32_t>(create_info.vertex_input_binding_descriptions.size()),
					.pVertexBindingDescriptions = create_info.vertex_input_binding_descriptions.data(),
					.vertexAttributeDescriptionCount = static_cast<uint32_t>(create_info.vertex_input_attribute_descriptions.size()),
					.pVertexAttributeDescriptions = create_info.vertex_input_attribute_descriptions.data()
				};

				create_info.ci_dynamic = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.dynamicStateCount = static_cast<uint32_t>(create_info.dynamics.size()),
					.pDynamicStates = create_info.dynamics.data()
				};

				create_info.ci_pipeline.pVertexInputState = &ci_vertex_input;
				create_info.ci_pipeline.pInputAssemblyState = &create_info.ci_input_assembly;
				create_info.ci_pipeline.pViewportState = &create_info.ci_viewport;
				create_info.ci_pipeline.pRasterizationState = &create_info.ci_rasterization;
				create_info.ci_pipeline.pMultisampleState = &create_info.ci_multisample;
				create_info.ci_pipeline.pDepthStencilState = &create_info.ci_depth_stencil;
				create_info.ci_pipeline.pColorBlendState = &create_info.ci_color_blend;
				create_info.ci_pipeline.pDynamicState = &create_info.ci_dynamic;

				VkResult res = VK_RESULT_MAX_ENUM;

				try {
					VkPipeline pipeline{};
					if (res = vkCreateGraphicsPipelines(GPUFactory::get_device(), nullptr, 1, &create_info.ci_pipeline, nullptr, &pipeline);
						res != VK_SUCCESS || !pipeline) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_render_pass = create_info.render_pass;
					obj->m_subpass_idx = create_info.subpass_idx;
					obj->m_pipeline = pipeline;
					obj->m_pipeline_layout = create_info.layout->m_layout;

					create_info.render_pass->m_pipelines.emplace_back(obj);
					m_manager->add(obj);
					return obj;
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

		private:
			CreateInfo create_info{};
			GPUResourceManager<target_type>* m_manager{};
		};

	}
}