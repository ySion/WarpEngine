#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUImage.h"
#include "Graphics/GPUResource/GPUFrameBuffer.h"
#include "Graphics/GPUResource/GPURenderPass.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUFrameBuffer> {
		public:
			using target_type = GPUFrameBuffer;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name{};
				VkFramebufferCreateInfo ci_framebuffer{
					.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.renderPass = nullptr,
					.attachmentCount = 0,
					.pAttachments = nullptr,
					.width = 0,
					.height = 0,
					.layers = 0
				};

				MVector<VkImageView> views{};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_render_pass(GPURenderPass* render_pass) {
				create_info.ci_framebuffer.renderPass = render_pass->m_render_pass;
				return *this;
			}

			constexpr self_type& set_extent(VkExtent2D v, uint32_t layers = 1) {
				create_info.ci_framebuffer.width = v.width;
				create_info.ci_framebuffer.height = v.height;
				create_info.ci_framebuffer.layers = layers;
				return *this;
			}

			constexpr self_type& set_extent(uint32_t w, uint32_t h, uint32_t layers = 1) {
				create_info.ci_framebuffer.width = w;
				create_info.ci_framebuffer.height = h;
				create_info.ci_framebuffer.layers = layers;
				return *this;
			}

			constexpr self_type& add_attachment(const GPUImage* image, uint32_t view_index = 0) {
				if(image) create_info.views.push_back(image->m_views[view_index]->m_image_view);
				return *this;
			}

			constexpr self_type& add_attachment(const GPUImageView* image_view) {
				if(image_view) create_info.views.push_back(image_view->m_image_view);
				return *this;
			}

			constexpr self_type& add_attachment(VkImageView image_view) {
				create_info.views.push_back(image_view);
				return *this;
			}

			target_type* make(bool replace = false) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				create_info.ci_framebuffer.attachmentCount = create_info.views.size();
				create_info.ci_framebuffer.pAttachments = create_info.views.data();

				VkFramebuffer frame_buffer{};

				VkResult res = VK_RESULT_MAX_ENUM;
				try{
					if (res = vkCreateFramebuffer(GPUFactory::get_device(), &create_info.ci_framebuffer, nullptr, &frame_buffer);
						VK_SUCCESS != res || !frame_buffer) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_framebuffer = frame_buffer;
					obj->m_extent = { create_info.ci_framebuffer.width, create_info.ci_framebuffer.height };
					obj->m_layers = create_info.ci_framebuffer.layers;

					for (int i = 0; i < create_info.views.size(); i++) {
						obj->m_attachments_view.push_back(create_info.views[i]);
					}

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
