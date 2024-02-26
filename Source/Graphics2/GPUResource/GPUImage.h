#pragma once

#include "GPUResourceBase.h"

namespace Warp {
	namespace GPU {
		class GPUImageView : public MIResource {
		public:
			struct Deleter {
				constexpr void operator()(const GPUImageView* ptr) const {
					if (ptr) {
						if (ptr->m_image_view) {
							vkDestroyImageView(GPUFactory::get_device(), ptr->m_image_view, nullptr);
						}
						delete ptr;
					}
				}
			};

			VkImageView m_image_view{};
			VkFormat m_format{};
			VkImageViewType m_image_view_type{};
			VkComponentMapping m_components{};
			VkImageSubresourceRange m_image_subresource_range{};
		};

		class GPUImage final : public GPUResource {
		public:
			inline GPUImage(const MString& name) : GPUResource("image", name) {}

			inline ~GPUImage() override {
				m_views.clear();
				vmaDestroyImage(GPUFactory::get_vma(), m_image, m_memory);
			}

			GPUImageView* add_view(VkImageViewType view_type, VkFormat format, const VkComponentMapping& components, const VkImageSubresourceRange& subresource_range);

			GPUImageView* add_view(VkImageViewType view_type, const VkComponentMapping& components, const VkImageSubresourceRange& subresource_range);

			GPUImageView* add_view(const VkComponentMapping& components);

			GPUImageView* add_view(const VkImageSubresourceRange& subresource_range);

			GPUImageView* add_view(const VkImageViewCreateInfo& create_info);

			//Image Config
			VkImage m_image{};

			VkExtent3D m_extent{};

			VkFormat m_image_format{};

			VkImageType m_image_type{};

			VkSampleCountFlagBits m_sample{};

			VkImageTiling m_tiling{};

			VkImageUsageFlags m_usage{};

			uint32_t m_mip_count{};

			uint32_t m_layer_count{};

			//Memory Config
			VmaAllocation m_memory{};

			VmaMemoryUsage m_memory_usage{};

			VmaAllocationCreateFlags m_memory_flag{};

			VkImageLayout m_born_init_layout{};

			//Views
			MVector<std::unique_ptr<GPUImageView, GPUImageView::Deleter>> m_views{};
		};
	}
}