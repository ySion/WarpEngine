#pragma once

#include "VkBase.hpp"
#include "ImageView.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, Image)

namespace Warp::Gpu {


	class Device;

	class Image : public Inherit<Image, Object> {
	public:

		Image(Device* device, 
			VkFormat format,
			VkExtent3D extent,
			VkImageUsageFlags usage,
			VmaAllocationCreateFlags memory_flags,
			VkImageType imageType,
			uint32_t mip_levels = 1,
			uint32_t arrayLayers = 1,
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
			VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, 
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);

		~Image() override;


		ImageView* create_view(VkImageViewType viewType, VkFormat view_format, const VkComponentMapping& component, VkImageAspectFlags aspectMask, uint32_t
		                       baseMipLevel = 0, uint32_t levelCount = 1, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);

		Device* get_device() const { return _device; }

		inline VkImage vk() const { return _image; }

		operator VkImage() const { return _image; }

	private:
		Device* _device{};

		VkImage _image{};

		VmaAllocation _allocation{};
		
		MVector<std::unique_ptr<ImageView>> _image_views {};
	};
}