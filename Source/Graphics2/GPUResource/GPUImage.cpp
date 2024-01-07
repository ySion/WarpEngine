#include "GPUImage.h"

namespace Warp {
	namespace GPU {
		GPUImageView* GPUImage::add_view(VkImageViewType view_type, VkFormat format,
			const VkComponentMapping& components, const VkImageSubresourceRange& subresource_range) {
			VkImageViewCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = m_image,
				.viewType = view_type,
				.format = format,
				.components = components,
				.subresourceRange = subresource_range
			};
			return add_view(create_info);
		}

		GPUImageView* GPUImage::add_view(VkImageViewType view_type, const VkComponentMapping& components,
						 const VkImageSubresourceRange& subresource_range) {
			VkImageViewCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = m_image,
				.viewType = view_type,
				.format = m_views[0]->m_format,
				.components = components,
				.subresourceRange = subresource_range
			};
			return add_view(create_info);

		}

		GPUImageView* GPUImage::add_view(const VkComponentMapping& components) {
			VkImageViewCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = m_image,
				.viewType = m_views[0]->m_image_view_type,
				.format = m_views[0]->m_format,
				.components = components,
				.subresourceRange = m_views[0]->m_image_subresource_range
			};
			return add_view(create_info);
		}

		GPUImageView* GPUImage::add_view(const VkImageSubresourceRange& subresource_range) {
			VkImageViewCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = m_image,
				.viewType = m_views[0]->m_image_view_type,
				.format = m_views[0]->m_format,
				.components = m_views[0]->m_components,
				.subresourceRange = subresource_range
			};
			return add_view(create_info);
		}

		GPUImageView* GPUImage::add_view(const VkImageViewCreateInfo& create_info) {
			try {
				VkImageView image_view{};

				if (VK_SUCCESS != vkCreateImageView(GPUFactory::get_device(), &create_info, nullptr, &image_view)) {
					return nullptr;
				}

				auto* temp = new GPUImageView{
					.m_image_view = image_view,
					.m_format = create_info.format,
					.m_image_view_type = create_info.viewType,
					.m_components = create_info.components,
					.m_image_subresource_range = create_info.subresourceRange
				};

				m_views.emplace_back(temp);
				return temp;
			} catch (...) {
				return nullptr;
			}
		}
	}

}