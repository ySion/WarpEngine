#pragma once

#include "VulkanFiles.h"

#include "GPUFactory.h"
#include "GPUResource.h"
#include "GPUResourceManager.h"
#include "GPUHelper.h"
#include "GPUShaderCompiler.h"

#include <memory>
#include <string>
#include <type_traits>

#include "Core/Utils.h"

namespace Warp {
	namespace GPU {
		template<class T> struct GPUResourceBuildTemplate;

		template<typename T> class GPUResourceBuilder {};

		namespace GPUResourceTypes {
			using GPUImage = ::Warp::GPU::GPUImage;
			using GPUSwapChain = ::Warp::GPU::GPUSwapChain;
			using GPUCommandPool = ::Warp::GPU::GPUCommandPool;
			using GPUCommandBuffer = ::Warp::GPU::GPUCommandBuffer;
			using GPUFence = ::Warp::GPU::GPUFence;
			using GPUSemaphore = ::Warp::GPU::GPUSemaphore;
			using GPURenderPass = ::Warp::GPU::GPURenderPass;
			using GPUGraphicsPipeline = ::Warp::GPU::GPUGraphicsPipeline;
			using GPUShader = ::Warp::GPU::GPUShader;
			using GPUPipelineLayout = ::Warp::GPU::GPUPipelineLayout;
			using GPUFrameBuffer = ::Warp::GPU::GPUFrameBuffer;
			using GPUBuffer = ::Warp::GPU::GPUBuffer;
		}


		//Frame Buffer具有自行创建Image和View的能力

		template<>
		class GPUResourceBuilder<GPUShader> {
			GPUResourceManager<GPUShader>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUShader>;
			struct CreateInfo {
				MString m_name;
				VkShaderModuleCreateInfo m_module_create_info{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUShader>* manager, const MString& name) : m_manager(manager)
			{
				create_info.m_name = name;
			}

			GPUShader* make_from_file(const MString& file_name, EShLanguage stage) {
				if (create_info.m_name.empty()) {
					LOGE("[GPUResourceBuilder<{}>] file name is empty.", typeid(GPUShader).name());
					return nullptr;
				}

				MString codes{};
				try {
					codes = Warp::read_text_file(file_name);
				} catch (...) {
					LOGE("[GPUResourceBuilder<{}>] can't read from file: {}.", typeid(GPUShader).name(), file_name);
					return nullptr;
				}

				if (codes.empty()) {
					LOGE("[GPUResourceBuilder<{}>] file is empty.", typeid(GPUShader).name());
					return nullptr;
				}

				return make_from_string(codes, stage);
			}

			//always replaced
			GPUShader* make_from_string(const MString& codes, EShLanguage stage) {
				if (create_info.m_name.empty()) {
					return nullptr;
				}

				if (codes.empty()) {
					return nullptr;
				}

				std::unique_ptr<GPUShader> obj_ptr = std::make_unique<GPUShader>(create_info.m_name);

				MVector<uint32_t> spirv{};
				const MString errs = ShaderCompiler::compile_shader_from_string(create_info.m_name, codes, stage, spirv);
				if(!errs.empty())
				{
					LOGE("[GPUResourceBuilder<{}>] Shader Compile, {}.", typeid(GPUShader).name(), errs);
				}

				if (spirv.empty()) {
					LOGE("[GPUResourceBuilder<{}>] Shader Compile success, but null spriv.", typeid(GPUShader).name());
					return nullptr;
				}

				create_info.m_module_create_info = {
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.codeSize = spirv.size() * sizeof(uint32_t),
					.pCode = spirv.data()
				};

				VkShaderModule shader_module{};

				if (VK_SUCCESS != vkCreateShaderModule(GPUFactory::get_device(), &create_info.m_module_create_info, nullptr, &shader_module)) {
					LOGE("[GPUResourceBuilder<{}>] Create shader module failed.", typeid(GPUShader).name());
					return nullptr;
				}

				obj_ptr->m_code = codes;
				obj_ptr->m_stage = stage;
				obj_ptr->m_spirv = MVector<uint32_t>(spirv.begin(), spirv.end());
				obj_ptr->m_module = shader_module;

				const auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUDescriptorPool> {
			GPUResourceManager<GPUDescriptorPool>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUDescriptorPool>;
			struct CreateInfo {
				MString m_name;
				/*VkDescriptorPoolCreateInfo m_pipeline_layout_create_info{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.maxSets = GPUFactory::get_swap_chain_image_count(),
					.poolSizeCount = GPUFactory::get_swap_chain_image_count(),
					.pPoolSizes = 512
				};*/
			};

			GPUResourceBuilder(GPUResourceManager<GPUDescriptorPool>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
			}




		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUPipelineLayout> {
			GPUResourceManager<GPUPipelineLayout>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUPipelineLayout>;

			struct CreateInfo {
				MString m_name;
				VkPipelineLayoutCreateInfo m_pipeline_layout_create_info{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUPipelineLayout>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
			}

			GPUPipelineLayout* make(){
				VkPipelineLayout layout{};

				create_info.m_pipeline_layout_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.setLayoutCount = 0,
					.pSetLayouts = nullptr,
					.pushConstantRangeCount = 0,
					.pPushConstantRanges = nullptr
				};

				if (VK_SUCCESS != vkCreatePipelineLayout(GPUFactory::get_device(), &create_info.m_pipeline_layout_create_info, nullptr, &layout)) {
					return nullptr;
				}

				std::unique_ptr<GPUPipelineLayout> obj_ptr = std::make_unique<GPUPipelineLayout>(create_info.m_name);

				obj_ptr->m_layout = layout;
				
				auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));

				return temp_ptr;
			}

		private:
			CreateInfo create_info{};
		};


		template<>
		class GPUResourceBuilder<GPUGraphicsPipeline> {
			GPUResourceManager<GPUGraphicsPipeline>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUGraphicsPipeline>;
			struct CreateInfo {
				MString m_name;
				VkGraphicsPipelineCreateInfo m_graphics_pipeline_create_info{};
				GPUShader* m_vertex{};
				GPUShader* m_tess_control{};
				GPUShader* m_tess_evaluation{};
				GPUShader* m_geometry{};
				GPUShader* m_fragment{};

				GPUPipelineLayout* m_layout;

				GPURenderPass* m_render_pass_ref = nullptr;
				uint32_t m_subpass_idx = 0;

				MVector<VkPipelineShaderStageCreateInfo> m_stage_create_info{ };

				VkPipelineDynamicStateCreateInfo m_dynamic_state_create_info{};
				MVector<VkDynamicState> m_dynamic_states{};


				MVector<VkVertexInputBindingDescription> m_vertex_input_binding_descriptions{};
				MVector<VkVertexInputAttributeDescription> m_vertex_input_attribute_descriptions{};

				VkPipelineInputAssemblyStateCreateInfo m_pipeline_input_assembly_state_create_info{};

				MVector<VkRect2D> m_scissors{};
				MVector<VkViewport> m_view_ports{};
				VkPipelineViewportStateCreateInfo m_pipeline_viewport_state_create_info{};

				VkPipelineRasterizationStateCreateInfo m_pipeline_rasterization_state_create_info{};

				VkPipelineMultisampleStateCreateInfo m_pipeline_multisample_state_create_info{};

				VkPipelineDepthStencilStateCreateInfo m_pipeline_depth_stencil_state_create_info{};

				MVector<VkPipelineColorBlendAttachmentState> m_pipeline_color_blend_attachment_states{};
				VkPipelineColorBlendStateCreateInfo m_pipeline_color_blend_state_create_info{};

			};

			GPUResourceBuilder(GPUResourceManager<GPUGraphicsPipeline>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
				create_info.m_graphics_pipeline_create_info = {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,		//OK
					.flags = 0,			//OK
					.stageCount = 0,		//OK
					.pStages = nullptr,		//OK
					.pVertexInputState = nullptr,	//OK
					.pInputAssemblyState = nullptr,	//OK
					.pTessellationState = nullptr,	//OK //no support
					.pViewportState = nullptr,	//OK
					.pRasterizationState = nullptr, //OK
					.pMultisampleState = nullptr,	//OK
					.pDepthStencilState = nullptr,	//OK
					.pColorBlendState = nullptr,	//OK
					.pDynamicState = nullptr,	//OK
					.layout = nullptr,		//OK
					.renderPass = nullptr,		//OK
					.subpass = 0,			//OK
					.basePipelineHandle = nullptr,	//OK
					.basePipelineIndex = 0		//OK
				};

				create_info.m_scissors.push_back({ .offset = {0,0}, .extent = {256, 256} });
				create_info.m_view_ports.push_back({ .x = 0, .y = 0, .width = 256 , .height = 256, .minDepth = 0.0f, .maxDepth = 1.0f });

				create_info.m_pipeline_viewport_state_create_info = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.viewportCount = (uint32_t)create_info.m_view_ports.size(),
						.pViewports = create_info.m_view_ports.data(),
						.scissorCount = (uint32_t)create_info.m_scissors.size(),
						.pScissors = create_info.m_scissors.data()
				};

				create_info.m_pipeline_depth_stencil_state_create_info = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.depthTestEnable = false,
						.depthWriteEnable = false,
						.depthCompareOp = VK_COMPARE_OP_LESS,
						.depthBoundsTestEnable = false,
						.stencilTestEnable = false,
						.front = VkStencilOp::VK_STENCIL_OP_KEEP,
						.back = VkStencilOp::VK_STENCIL_OP_KEEP,
						.minDepthBounds = 0.0,
						.maxDepthBounds = 0.0
				};

				create_info.m_pipeline_multisample_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
					.sampleShadingEnable = false,
					.minSampleShading = 1.0f,
				};

				create_info.m_pipeline_color_blend_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.logicOpEnable = false,
					.logicOp = VK_LOGIC_OP_CLEAR,
					.attachmentCount = 0,
					.pAttachments = nullptr,
					.blendConstants = {0.0f,0.0f,0.0f,0.0f}
				};

				create_info.m_dynamic_states.push_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);
				create_info.m_dynamic_states.push_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

			}


			constexpr self_type& set_render_pass(GPURenderPass* render_pass, uint32_t subpass_idx) {
				create_info.m_render_pass_ref = render_pass;
				create_info.m_subpass_idx = subpass_idx;

				create_info.m_graphics_pipeline_create_info.renderPass = render_pass->m_render_pass;
				create_info.m_graphics_pipeline_create_info.subpass = subpass_idx;
				return *this;
			}

			//自动开启VK_DYNAMIC_STATE_VIEWPORT和VK_DYNAMIC_STATE_SCISSOR
			constexpr self_type& add_dynamic_state(const MVector<VkDynamicState>& states) {

				create_info.m_dynamic_states = states;

				create_info.m_dynamic_states.push_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);
				create_info.m_dynamic_states.push_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

				return *this;
			}

			constexpr self_type& add_vertex_buffer(const VkVertexInputBindingDescription& binding_desc, const MVector<VkVertexInputAttributeDescription>& attributes) {

				create_info.m_vertex_input_binding_descriptions.push_back(binding_desc);
				create_info.m_vertex_input_attribute_descriptions.insert(create_info.m_vertex_input_attribute_descriptions.end(),
					attributes.begin(), attributes.end());

				return *this;
			}

			constexpr self_type& set_input_assembly_state(VkPrimitiveTopology topology, bool primitive_restart_enable = false) {

				create_info.m_pipeline_input_assembly_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.topology = topology,
					.primitiveRestartEnable = primitive_restart_enable
				};
				return *this;
			}

			constexpr self_type& set_layout(GPUPipelineLayout* layout) {
				create_info.m_layout = layout;
				create_info.m_graphics_pipeline_create_info.layout = layout->m_layout;
				return *this;
			}

			constexpr self_type& set_viewport_count(uint32_t count) {
				create_info.m_scissors.clear();
				create_info.m_view_ports.clear();

				for (int i = 0; i < count; ++i) {
					create_info.m_scissors.push_back({ .offset = {0,0}, .extent = {256, 256} });
					create_info.m_view_ports.push_back({ .x = 0, .y = 0, .width = 256 , .height = 256, .minDepth = 0.0f, .maxDepth = 1.0f });
				}

				create_info.m_pipeline_viewport_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.viewportCount = (uint32_t)create_info.m_view_ports.size(),
					.pViewports = create_info.m_view_ports.data(),
					.scissorCount = (uint32_t)create_info.m_scissors.size(),
					.pScissors = create_info.m_scissors.data()
				};

				return *this;
			}

			constexpr self_type& set_rasterization_state(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT, VkFrontFace front_face = VK_FRONT_FACE_CLOCKWISE) {
				create_info.m_pipeline_rasterization_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO ,
					.pNext = nullptr,
					.flags = 0,
					.depthClampEnable = false,
					.rasterizerDiscardEnable = false,
					.polygonMode = polygon_mode,
					.cullMode = cull_mode,
					.frontFace = front_face,
					.depthBiasEnable = false,
					.depthBiasConstantFactor = 0,
					.depthBiasClamp = 0,
					.depthBiasSlopeFactor = 0,
					.lineWidth = 1.0f
				};
				return *this;
			}

			constexpr self_type& set_multisample_state(VkSampleCountFlagBits sample_count) {
				create_info.m_pipeline_multisample_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.rasterizationSamples = sample_count,
					.sampleShadingEnable = false,
					.minSampleShading = 1.0f,
				};
				return *this;
			}

			constexpr self_type& set_depth_state(bool depth_test, bool depth_write = true, VkCompareOp pixel_new_op_old = VK_COMPARE_OP_LESS) {
				create_info.m_pipeline_depth_stencil_state_create_info.depthTestEnable = depth_test;
				create_info.m_pipeline_depth_stencil_state_create_info.depthWriteEnable = depth_write;
				create_info.m_pipeline_depth_stencil_state_create_info.depthCompareOp = pixel_new_op_old;
				return *this;
			}

			constexpr self_type& set_stencil_state(bool stencil_test, VkStencilOpState front_op, VkStencilOpState back_op) {
				create_info.m_pipeline_depth_stencil_state_create_info.stencilTestEnable = stencil_test;
				create_info.m_pipeline_depth_stencil_state_create_info.front = front_op;
				create_info.m_pipeline_depth_stencil_state_create_info.back = back_op;
				return *this;
			}

			constexpr self_type& set_blend_bounds_test(bool depth_bounds_test, float min_depth_bounds, float max_depth_bounds) {
				create_info.m_pipeline_depth_stencil_state_create_info.depthBoundsTestEnable = depth_bounds_test;
				create_info.m_pipeline_depth_stencil_state_create_info.minDepthBounds = min_depth_bounds;
				create_info.m_pipeline_depth_stencil_state_create_info.maxDepthBounds = max_depth_bounds;
				return *this;
			}

			constexpr self_type& add_color_blend_attachment_state(bool blend_enable = false,
				VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_SRC_ALPHA,
				VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VkBlendOp color_blend_op = VK_BLEND_OP_ADD,
				VkBlendFactor src_alpha_blend = VK_BLEND_FACTOR_SRC_ALPHA,
				VkBlendFactor dst_alpha_blend = VK_BLEND_FACTOR_DST_ALPHA,
				VkBlendOp alpha_blend_op = VK_BLEND_OP_ADD,
				VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			) {
				VkPipelineColorBlendAttachmentState stat{
						.blendEnable = false,
						.srcColorBlendFactor = src_color_blend_factor,
						.dstColorBlendFactor = dst_color_blend_factor,
						.colorBlendOp = color_blend_op,
						.srcAlphaBlendFactor = src_alpha_blend,
						.dstAlphaBlendFactor = dst_alpha_blend,
						.alphaBlendOp = alpha_blend_op,
						.colorWriteMask = color_write_mask
				};
				create_info.m_pipeline_color_blend_attachment_states.push_back(stat);
				return *this;
			}

			constexpr self_type& set_color_blend_state(bool enable_logic_op = false,
				VkLogicOp logic_op = VK_LOGIC_OP_COPY, const std::array<float, 4>& blend_constans = {0,0,0,0}
			) {

				create_info.m_pipeline_color_blend_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.logicOpEnable = enable_logic_op,
					.logicOp = logic_op,
					.attachmentCount = (uint32_t)create_info.m_pipeline_color_blend_attachment_states.size(),
					.pAttachments = create_info.m_pipeline_color_blend_attachment_states.data(),
				};
				create_info.m_pipeline_color_blend_state_create_info.blendConstants[0] = blend_constans[0];
				create_info.m_pipeline_color_blend_state_create_info.blendConstants[1] = blend_constans[1];
				create_info.m_pipeline_color_blend_state_create_info.blendConstants[2] = blend_constans[2];
				create_info.m_pipeline_color_blend_state_create_info.blendConstants[3] = blend_constans[3];
				return *this;
			}

			constexpr self_type& set_shaders(
					GPUShader* vertex = nullptr,
					GPUShader* tess_control = nullptr,
					GPUShader* tess_evaluation = nullptr,
					GPUShader* geometry = nullptr,
					GPUShader* fragment = nullptr)
			{

				create_info.m_stage_create_info.clear();

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
					create_info.m_stage_create_info.push_back(stage_create_info);
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
					create_info.m_stage_create_info.push_back(stage_create_info);
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
					create_info.m_stage_create_info.push_back(stage_create_info);
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
					create_info.m_stage_create_info.push_back(stage_create_info);
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
					create_info.m_stage_create_info.push_back(stage_create_info);
				}


				create_info.m_vertex = vertex;
				create_info.m_tess_control = tess_control;
				create_info.m_tess_evaluation = tess_evaluation;
				create_info.m_geometry = geometry;
				create_info.m_fragment = fragment;

				create_info.m_graphics_pipeline_create_info.stageCount = (uint32_t)create_info.m_stage_create_info.size();
				create_info.m_graphics_pipeline_create_info.pStages = create_info.m_stage_create_info.data();
				return *this;
			}

			GPUGraphicsPipeline* make(bool replace = true) {

				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>]Pipeline \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(GPUGraphicsPipeline).name(), create_info.m_name);
					return nullptr;
				}

				create_info.m_dynamic_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.dynamicStateCount = (uint32_t)create_info.m_dynamic_states.size(),
					.pDynamicStates = create_info.m_dynamic_states.data()
				};
				create_info.m_graphics_pipeline_create_info.pDynamicState = &create_info.m_dynamic_state_create_info;

				VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.pNext = nullptr ,
					.flags = 0,
					.vertexBindingDescriptionCount = (uint32_t)create_info.m_vertex_input_binding_descriptions.size(),
					.pVertexBindingDescriptions = create_info.m_vertex_input_binding_descriptions.data(),
					.vertexAttributeDescriptionCount = (uint32_t)create_info.m_vertex_input_attribute_descriptions.size(),
					.pVertexAttributeDescriptions = create_info.m_vertex_input_attribute_descriptions.data()
				};

				create_info.m_graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
				create_info.m_graphics_pipeline_create_info.pInputAssemblyState = &create_info.m_pipeline_input_assembly_state_create_info;
				create_info.m_graphics_pipeline_create_info.pViewportState = &create_info.m_pipeline_viewport_state_create_info;
				create_info.m_graphics_pipeline_create_info.pRasterizationState = &create_info.m_pipeline_rasterization_state_create_info;
				create_info.m_graphics_pipeline_create_info.pMultisampleState = &create_info.m_pipeline_multisample_state_create_info;
				create_info.m_graphics_pipeline_create_info.pDepthStencilState = &create_info.m_pipeline_depth_stencil_state_create_info;
				create_info.m_graphics_pipeline_create_info.pColorBlendState = &create_info.m_pipeline_color_blend_state_create_info;

				VkPipeline pipeline{};
				if(VK_SUCCESS != vkCreateGraphicsPipelines(GPUFactory::get_device(), nullptr, 1, &create_info.m_graphics_pipeline_create_info, nullptr, &pipeline))
				{
					return nullptr;
				}

				std::unique_ptr<GPUGraphicsPipeline> obj_ptr = std::make_unique<GPUGraphicsPipeline>(create_info.m_name);
				auto temp_ptr = obj_ptr.get();
				obj_ptr->m_render_pass = create_info.m_render_pass_ref;
				obj_ptr->m_subpass_idx = create_info.m_subpass_idx;
				obj_ptr->m_pipeline = pipeline;

				create_info.m_render_pass_ref->m_pipelines.emplace_back(temp_ptr);
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPURenderPass> {
			GPUResourceManager<GPURenderPass>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPURenderPass>;

			struct SubPassCreateInfo {
				MVector<uint32_t> m_color_attachment_refs{};
				MVector<uint32_t> m_input_attachment_refs{};
				MVector<uint32_t> m_resolve_attachment_refs{};
				std::optional<uint32_t> m_depth_stencil_resolve_attachment_ref = { std::nullopt };
				MVector<uint32_t> m_preserve_attachment_refs{};
				VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
			};

			struct CreateInfo {
				MString m_name;
				MVector<VkAttachmentDescription> m_attachment_descriptions{};
				MVector<VkSubpassDependency> m_subpass_dependencies{};
				MVector<SubPassCreateInfo> m_subpass_create_infos{};
			};

			GPUResourceBuilder(GPUResourceManager<GPURenderPass>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
			}


			//need attachment
			//need subpasses
			//need dependencies

			constexpr self_type& add_subpass(const SubPassCreateInfo& subpass_create_info) {
				create_info.m_subpass_create_infos.emplace_back(subpass_create_info);
				return *this;
			}

			constexpr self_type& add_subpass_dependency(const VkSubpassDependency& subpass_dependency) {
				create_info.m_subpass_dependencies.emplace_back(subpass_dependency);
				return *this;
			}

			self_type& add_attachment_decltype(const GPUImage& template_image, VkAttachmentLoadOp load_op, VkAttachmentStoreOp storeOp, VkImageLayout init_layout = VK_IMAGE_LAYOUT_UNDEFINED) {

				VkImageLayout default_layout{};

				if (is_depth_stencil_format(template_image.m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_stencil_only_format(template_image.m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_depth_only_format(template_image.m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				} else {
					default_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				create_info.m_attachment_descriptions.emplace_back(VkAttachmentDescription{
					.flags = 0,
					.format = template_image.m_image_format,
					.samples = template_image.m_sample,
					.loadOp = load_op,
					.storeOp = storeOp,
					.stencilLoadOp = load_op,
					.stencilStoreOp = storeOp,
					.initialLayout = init_layout == VK_IMAGE_LAYOUT_UNDEFINED ? default_layout : init_layout,
					.finalLayout = default_layout
					});

				return *this;
			}

			self_type& add_attachment(VkFormat format, VkSampleCountFlagBits flags, VkAttachmentLoadOp load_op, VkAttachmentStoreOp storeOp, VkImageLayout init_layout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED) {

				VkImageLayout default_layout{};

				/*if (is_depth_stencil_format(format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_stencil_only_format(format)) {
					default_layout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_depth_only_format(format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				} else {
					default_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}*/

				create_info.m_attachment_descriptions.emplace_back(VkAttachmentDescription{
					.flags = 0,
					.format = format,
					.samples = flags,
					.loadOp = load_op,
					.storeOp = storeOp,
					.stencilLoadOp = load_op,
					.stencilStoreOp = storeOp,
					.initialLayout = init_layout,
					.finalLayout = final_layout 
					});

				return *this;
			}


			GPURenderPass* make() {
				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, RenderPass can't be replaced make.", typeid(GPURenderPass).name(), create_info.m_name);
					return nullptr;
				}

				MVector<VkSubpassDescription> subpass_descriptions{};

				uint32_t subpass_count = create_info.m_subpass_create_infos.size();
				MVector<MVector<VkAttachmentReference>> subpass_input_attachment_refs{ subpass_count };
				MVector<MVector<VkAttachmentReference>> subpass_color_attachment_refs{ subpass_count };
				MVector<MVector<VkAttachmentReference>> subpass_resolve_attachment_refs{ subpass_count };
				MVector<VkAttachmentReference> subpass_depth_stencil_attachment_refs{ subpass_count };

				int i = 0;
				for (const auto& subpass_create_info : create_info.m_subpass_create_infos) {
					MVector<VkAttachmentReference>& input_attachment_refs = subpass_input_attachment_refs[i];
					MVector<VkAttachmentReference>& color_attachment_refs = subpass_color_attachment_refs[i];
					MVector<VkAttachmentReference>& resolve_attachment_refs = subpass_resolve_attachment_refs[i];
					VkAttachmentReference& depth_stencil_attachment_refs = subpass_depth_stencil_attachment_refs[i];

					for (const auto i : subpass_create_info.m_input_attachment_refs) {
						const auto& format = create_info.m_attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout_read_only(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						input_attachment_refs.emplace_back(ref);
					}

					for (const auto i : subpass_create_info.m_color_attachment_refs) {
						const auto& format = create_info.m_attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						color_attachment_refs.emplace_back(ref);
					}

					for (const auto i : subpass_create_info.m_resolve_attachment_refs) {
						const auto& format = create_info.m_attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						resolve_attachment_refs.emplace_back(ref);
					}

					bool has_depth_stencil = false;
					if (subpass_create_info.m_depth_stencil_resolve_attachment_ref.has_value()) {
						uint32_t i = subpass_create_info.m_depth_stencil_resolve_attachment_ref.value();
						const auto& format = create_info.m_attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout(format);

						depth_stencil_attachment_refs = {
							.attachment = i,
							.layout = layout
						};
						has_depth_stencil = true;
					}

					VkSubpassDescription desc{
						.flags = 0,
						.pipelineBindPoint = subpass_create_info.bind_point,
						.inputAttachmentCount = (uint32_t)input_attachment_refs.size(),
						.pInputAttachments = input_attachment_refs.data(),  //输入的attachement
						.colorAttachmentCount = (uint32_t)color_attachment_refs.size(), //颜色输出的attachement
						.pColorAttachments = color_attachment_refs.data(),
						.pResolveAttachments = resolve_attachment_refs.data(), //包含多重采样的attachment, 不要传入多重采样的depth/stencil attachment, 暂时没有做支持
						.pDepthStencilAttachment = has_depth_stencil ? &depth_stencil_attachment_refs : nullptr, //depth/stencil的attachment
						.preserveAttachmentCount = (uint32_t)subpass_create_info.m_preserve_attachment_refs.size(),
						.pPreserveAttachments = subpass_create_info.m_preserve_attachment_refs.data()
					};

					subpass_descriptions.emplace_back(desc);
					i++;
				}

				//默认的粗粒度依赖
				if (create_info.m_subpass_create_infos.size() > 1) {
					if (create_info.m_subpass_dependencies.size() != create_info.m_subpass_create_infos.size()) {
						const uint32_t dependency_size = create_info.m_subpass_create_infos.size() - 1;
						for (uint32_t i = 0; i < dependency_size; ++i) {
							create_info.m_subpass_dependencies.emplace_back(VkSubpassDependency{
								.srcSubpass = i,
								.dstSubpass = i + 1,
								.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
								.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
								.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
								.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
								});
						}
					}
				}

				VkRenderPassCreateInfo render_pass_create_info{
					.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.attachmentCount = (uint32_t)create_info.m_attachment_descriptions.size(),
					.pAttachments = create_info.m_attachment_descriptions.data(),
					.subpassCount = (uint32_t)subpass_descriptions.size(),
					.pSubpasses = subpass_descriptions.data(),
					.dependencyCount = (uint32_t)create_info.m_subpass_dependencies.size(),
					.pDependencies = create_info.m_subpass_dependencies.data()
				};
				VkRenderPass render_pass;

				if (auto res = vkCreateRenderPass(GPUFactory::get_device(), &render_pass_create_info, nullptr, &render_pass);
					res != VK_SUCCESS) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] RenderPass create failed, return code {} {}.", typeid(GPURenderPass).name(), code_desc, static_cast<int>(res));
					return nullptr;
				}

				std::unique_ptr<GPURenderPass> obj_ptr = std::make_unique<GPURenderPass>(create_info.m_name);

				obj_ptr->m_render_pass = render_pass;
				obj_ptr->m_attachment_descriptions = MVector<VkAttachmentDescription>(create_info.m_attachment_descriptions.begin(), create_info.m_attachment_descriptions.end());

				auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}


		public:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUSemaphore> {
			GPUResourceManager<GPUSemaphore>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUSemaphore>;

			struct CreateInfo {
				MString m_name;
				VkSemaphore m_semaphore{};
				VkSemaphoreCreateInfo m_semaphore_create_info{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUSemaphore>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
				create_info.m_semaphore_create_info = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0
				};
			}


			GPUSemaphore* make() {

				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, Semaphore can't be replaced make.", typeid(GPUSemaphore).name(), create_info.m_name);
					return nullptr;
				}

				if (auto res = vkCreateSemaphore(GPUFactory::get_device(), &create_info.m_semaphore_create_info, nullptr, &create_info.m_semaphore);
					VK_SUCCESS != res) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Semaphore create failed, return code: {}.", typeid(GPUSemaphore).name(), code_desc, static_cast<int>(res));
					return nullptr;
				}

				std::unique_ptr<GPUSemaphore> obj_ptr = std::make_unique<GPUSemaphore>(create_info.m_name);
				obj_ptr->m_semaphore = create_info.m_semaphore;

				const auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

			MVector<GPUSemaphore*> make_multi(uint32_t count, bool all_anonymous_name = false) {
				MVector<GPUSemaphore*> res{ };

				if (count == 0) {
					return res;
				}

				res.resize(count);

				if (all_anonymous_name || create_info.m_name.empty()) {
						create_info.m_name = "";
					for (size_t i = 0; i < count; i++) {
						res[i] = make();
					}
				} else {
					MString origin_name = create_info.m_name;

					for (size_t i = 0; i < count; i++) {
						create_info.m_name = std::format("{}_{}", origin_name, i);
						res[i] = make();
					}
				}
				return res;
			}

		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUFence> {
			GPUResourceManager<GPUFence>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUFence>;

			struct CreateInfo {
				MString m_name;
				VkFence m_fence{};
				VkFenceCreateInfo m_vk_fence_create_info{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUFence>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
				create_info.m_vk_fence_create_info = {
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_FENCE_CREATE_SIGNALED_BIT
				};
			}

			constexpr  self_type& set_signal(bool signal = true) {
				if (signal) {
					create_info.m_vk_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
				} else {
					create_info.m_vk_fence_create_info.flags = 0;
				}
				return *this;
			}


			GPUFence* make() {

				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, Fence can't be replaced make.", typeid(GPUFence).name(), create_info.m_name);
					return nullptr;
				}

				if (auto res = vkCreateFence(GPUFactory::get_device(), &create_info.m_vk_fence_create_info, nullptr, &create_info.m_fence);
					res != VK_SUCCESS) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Fence create failed, return code {} {}.", typeid(GPUFence).name(), code_desc, static_cast<int>(res));
					return nullptr;
				}

				auto obj_ptr = std::make_unique<GPUFence>(create_info.m_name);
				obj_ptr->m_fence = create_info.m_fence;

				const auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

			//make_multi
			MVector<GPUFence*> make_multi(uint32_t count, bool all_anonymous_name = false) {

				MVector<GPUFence*> res{ };

				if (count == 0) {
					return res;
				}

				res.resize(count);

				if (all_anonymous_name || create_info.m_name.empty()) {
					create_info.m_name = "";
					for (size_t i = 0; i < count; i++) {
						res[i] = make();
					}
				} else {
					MString origin_name = create_info.m_name;

					for (size_t i = 0; i < count; i++) {
						create_info.m_name = std::format("{}_{}", origin_name, i);
						res[i] = make();
					}
				}
				return res;
			}

		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUCommandPool> {
			GPUResourceManager<GPUCommandPool>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUCommandPool>;

			struct CreateInfo {
				MString m_name{};
				VkCommandPoolCreateInfo m_vk_command_pool_create_info{};
				VkCommandPool m_command_pool{};
			};


			GPUResourceBuilder(GPUResourceManager<GPUCommandPool>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
				create_info.m_vk_command_pool_create_info = {
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
					.queueFamilyIndex = 0
				};
			}

			constexpr self_type& set_queue_family_index(uint32_t index) {
				create_info.m_vk_command_pool_create_info.queueFamilyIndex = index;
				return *this;
			}

			//flag
			constexpr self_type& set_flag(VkCommandPoolCreateFlags flags) {
				create_info.m_vk_command_pool_create_info.flags = flags;
				return *this;
			}

			//make
			GPUCommandPool* make() {
				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, CommandPool can't be replaced make.", typeid(GPUCommandPool).name(), create_info.m_name);
					return nullptr;
				}

				if (auto res = vkCreateCommandPool(GPUFactory::get_device(), &create_info.m_vk_command_pool_create_info, nullptr, &create_info.m_command_pool);
					res != VK_SUCCESS) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] CommandPool create failed, return code {} {}.", typeid(GPUCommandPool).name(), code_desc, static_cast<int>(res));
					return nullptr;
				}

				auto obj_ptr = std::make_unique<GPUCommandPool>(create_info.m_name);
				obj_ptr->m_command_pool = create_info.m_command_pool;
				obj_ptr->m_queue_family_index = create_info.m_vk_command_pool_create_info.queueFamilyIndex;
				obj_ptr->m_flag = create_info.m_vk_command_pool_create_info.flags;

				auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

		private:
			CreateInfo create_info{};
		};

		template<>
		class GPUResourceBuilder<GPUCommandBuffer> {
			GPUResourceManager<GPUCommandBuffer>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUCommandBuffer>;

			struct CreateInfo {
				MString m_name{};
				VkCommandBufferAllocateInfo m_vk_command_buffer_allocate_info{};
				VkCommandBuffer m_command_buffer{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUCommandBuffer>* manager, const MString& name) : m_manager(manager) {
				create_info.m_name = name;
				create_info.m_vk_command_buffer_allocate_info = {
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.pNext = nullptr,
					.commandPool = VK_NULL_HANDLE,
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1
				};
			}

			constexpr self_type& set_command_pool(VkCommandPool pool) {
				create_info.m_vk_command_buffer_allocate_info.commandPool = pool;
				return *this;
			}

			constexpr self_type& set_command_pool(const GPUCommandPool* pool) {
				create_info.m_vk_command_buffer_allocate_info.commandPool = pool->m_command_pool;
				return *this;
			}

			constexpr self_type& set_level(VkCommandBufferLevel level) {
				create_info.m_vk_command_buffer_allocate_info.level = level;
				return *this;
			}

			GPUCommandBuffer* make() {
				if (!create_info.m_name.empty() && m_manager->find_by_name(create_info.m_name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, CommandBuffer can't be replaced make.", typeid(GPUCommandBuffer).name(), create_info.m_name);
					return nullptr;
				}

				if (auto res = vkAllocateCommandBuffers(GPUFactory::get_device(), &create_info.m_vk_command_buffer_allocate_info, &create_info.m_command_buffer);
					res != VK_SUCCESS) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] CommandBuffer create failed, return code {} {}.", typeid(GPUCommandBuffer).name(), code_desc, static_cast<int>(res));
					return nullptr;
				}

				auto obj_ptr = std::make_unique<GPUCommandBuffer>(create_info.m_name);
				obj_ptr->m_command_buffer = create_info.m_command_buffer;
				obj_ptr->m_command_pool = create_info.m_vk_command_buffer_allocate_info.commandPool;
				obj_ptr->m_level = create_info.m_vk_command_buffer_allocate_info.level;
				obj_ptr->m_submit_info = {
					.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.pNext = nullptr,
					.waitSemaphoreCount = 0,
					.pWaitSemaphores = nullptr,
					.pWaitDstStageMask = nullptr,
					.commandBufferCount = 1,
					.pCommandBuffers = &obj_ptr->m_command_buffer,
					.signalSemaphoreCount = 0,
					.pSignalSemaphores = nullptr
				};
				auto temp_ptr = obj_ptr.get();
				m_manager->add(std::move(obj_ptr));
				return temp_ptr;
			}

			MVector<GPUCommandBuffer*> make_multi(uint32_t count, bool all_anonymous_name = false);

			//临时命令队列不会被管理, 调用完会立刻释放
			void make_and_execute_single_time_command_buffer(const std::function<void(GPUCommandBuffer& command_buffer)>& call_back);

		public:
			CreateInfo create_info{};
		};
	}
}
