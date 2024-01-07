#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPURenderPass.h"
#include "Graphics/GPUResource/GPUImage.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPURenderPass> {
		public:
			using target_type = GPURenderPass;
			using self_type = GPUResourceBuilder<target_type>;

			struct SubPassCreateInfo {
				MVector<uint32_t> color_attachment_refs{};
				MVector<uint32_t> input_attachment_refs{};
				MVector<uint32_t> resolve_attachment_refs{};
				std::optional<uint32_t> depth_stencil_resolve_attachment_ref = { std::nullopt };
				MVector<uint32_t> preserve_attachment_refs{};
				VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
			};

			struct CreateInfo {
				MString name;
				MVector<VkAttachmentDescription> attachment_descriptions{};
				MVector<VkSubpassDependency> subpass_dependencies{};
				MVector<SubPassCreateInfo> ci_subpass{};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			constexpr self_type& add_subpass(const SubPassCreateInfo& subpass_create_info) {
				create_info.ci_subpass.emplace_back(subpass_create_info);
				return *this;
			}

			constexpr self_type& add_subpass_dependency(const VkSubpassDependency& subpass_dependency) {
				create_info.subpass_dependencies.emplace_back(subpass_dependency);
				return *this;
			}

			self_type& add_attachment(VkFormat format, VkSampleCountFlagBits flags, VkAttachmentLoadOp load_op, VkAttachmentStoreOp storeOp, VkImageLayout init_layout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED) {

				VkImageLayout default_layout{};

				create_info.attachment_descriptions.emplace_back(VkAttachmentDescription{
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

			self_type& add_attachment_decltype(const GPUImage* template_image, VkAttachmentLoadOp load_op, VkAttachmentStoreOp storeOp, VkImageLayout init_layout = VK_IMAGE_LAYOUT_UNDEFINED) {

				VkImageLayout default_layout{};

				if (is_depth_stencil_format(template_image->m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_stencil_only_format(template_image->m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
				} else if (is_depth_only_format(template_image->m_image_format)) {
					default_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				} else {
					default_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				create_info.attachment_descriptions.emplace_back(VkAttachmentDescription{
					.flags = 0,
					.format = template_image->m_image_format,
					.samples = template_image->m_sample,
					.loadOp = load_op,
					.storeOp = storeOp,
					.stencilLoadOp = load_op,
					.stencilStoreOp = storeOp,
					.initialLayout = init_layout == VK_IMAGE_LAYOUT_UNDEFINED ? default_layout : init_layout,
					.finalLayout = default_layout
				});

				return *this;
			}

			target_type* make(bool replace = false) {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				MVector<VkSubpassDescription> subpass_descriptions{};

				uint32_t subpass_count = create_info.ci_subpass.size();
				MVector<MVector<VkAttachmentReference>> subpass_input_attachment_refs{ subpass_count };
				MVector<MVector<VkAttachmentReference>> subpass_color_attachment_refs{ subpass_count };
				MVector<MVector<VkAttachmentReference>> subpass_resolve_attachment_refs{ subpass_count };
				MVector<VkAttachmentReference> subpass_depth_stencil_attachment_refs{ subpass_count };

				int i = 0;
				for (const auto& subpass_create_info : create_info.ci_subpass) {
					MVector<VkAttachmentReference>& input_attachment_refs = subpass_input_attachment_refs[i];
					MVector<VkAttachmentReference>& color_attachment_refs = subpass_color_attachment_refs[i];
					MVector<VkAttachmentReference>& resolve_attachment_refs = subpass_resolve_attachment_refs[i];
					VkAttachmentReference& depth_stencil_attachment_refs = subpass_depth_stencil_attachment_refs[i];

					for (const auto i : subpass_create_info.input_attachment_refs) {
						const auto& format = create_info.attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout_read_only(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						input_attachment_refs.emplace_back(ref);
					}

					for (const auto i : subpass_create_info.color_attachment_refs) {
						const auto& format = create_info.attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						color_attachment_refs.emplace_back(ref);
					}

					for (const auto i : subpass_create_info.resolve_attachment_refs) {
						const auto& format = create_info.attachment_descriptions[i].format;
						const VkImageLayout layout = get_image_layout(format);

						VkAttachmentReference ref{
							.attachment = i,
							.layout = layout
						};

						resolve_attachment_refs.emplace_back(ref);
					}

					bool has_depth_stencil = false;
					if (subpass_create_info.depth_stencil_resolve_attachment_ref.has_value()) {
						uint32_t i = subpass_create_info.depth_stencil_resolve_attachment_ref.value();
						const auto& format = create_info.attachment_descriptions[i].format;
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
						.preserveAttachmentCount = (uint32_t)subpass_create_info.preserve_attachment_refs.size(),
						.pPreserveAttachments = subpass_create_info.preserve_attachment_refs.data()
					};

					subpass_descriptions.emplace_back(desc);
					i++;
				}

				//默认的粗粒度依赖
				if (create_info.ci_subpass.size() > 1) {
					if (create_info.subpass_dependencies.size() != create_info.ci_subpass.size()) {
						const uint32_t dependency_size = create_info.ci_subpass.size() - 1;
						for (uint32_t i = 0; i < dependency_size; ++i) {
							create_info.subpass_dependencies.emplace_back(VkSubpassDependency{
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

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkRenderPass render_pass{};
					const VkRenderPassCreateInfo ci_renderpass {
						.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.attachmentCount = static_cast<uint32_t>(create_info.attachment_descriptions.size()),
						.pAttachments = create_info.attachment_descriptions.data(),
						.subpassCount = static_cast<uint32_t>(subpass_descriptions.size()),
						.pSubpasses = subpass_descriptions.data(),
						.dependencyCount = static_cast<uint32_t>(create_info.subpass_dependencies.size()),
						.pDependencies = create_info.subpass_dependencies.data()
					};

					if (auto res = vkCreateRenderPass(GPUFactory::get_device(), &ci_renderpass, nullptr, &render_pass);
					res != VK_SUCCESS || !render_pass) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_render_pass = render_pass;
					obj->m_attachment_descriptions = MVector<VkAttachmentDescription>(create_info.attachment_descriptions.begin(), create_info.attachment_descriptions.end());

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
