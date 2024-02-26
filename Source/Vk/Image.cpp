#include "Image.hpp"

#include "Device.hpp"
#include "ImageView.hpp"

#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp::Gpu;

Image::Image(Device* device, VkFormat format,
	VkExtent3D extent,
	VkImageUsageFlags usage,
	VmaAllocationCreateFlags memory_flags,
	VkImageType imageType,
	uint32_t mip_levels,
	uint32_t arrayLayers,
	VkSampleCountFlagBits samples,
	VmaMemoryUsage memory_usage,
	VkImageTiling tiling, VkImageLayout layout) : _device(device) {

	VkImageCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = imageType,
		.format = format,
		.extent = extent,
		.mipLevels = mip_levels,
		.arrayLayers = arrayLayers,
		.samples = samples,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = layout
	};

	const VmaAllocationCreateInfo mci = {
		.flags = memory_flags,
		.usage = memory_usage
	};

	const VkResult result = vmaCreateImage(_device->get_allocator(), &ci, &mci, &_image, &_allocation, nullptr);

	if (result != VK_SUCCESS) {
		MString msg = MString::format("Failed to create image: {}, {}.", static_cast<int>(result), msg_map_VkResult(result));
		error(msg);
		throw Exception{ msg, result };
	}
}

Image::~Image() {

	for (auto& view : _image_views) {
		view.reset();
	}

	if (_image) {
		vkDestroyImage(_device->vk(), _image, nullptr);
	}
}

ImageView* Image::create_view(VkImageViewType viewType, VkFormat view_format, const VkComponentMapping& component, VkImageAspectFlags aspectMask, uint32_t baseMipLevel,
			      uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount) {
	VkImageViewCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = _image,
		.viewType = viewType,
		.format = view_format,
		.components = component,
		.subresourceRange = {
			.aspectMask = aspectMask,
			.baseMipLevel = baseMipLevel,
			.levelCount = levelCount,
			.baseArrayLayer = baseArrayLayer,
			.layerCount = layerCount
		}
	};

	VkImageView view;
	if (const VkResult res = vkCreateImageView(*_device, &ci, nullptr, &view); res != VK_SUCCESS) {
		MString msg = MString::format("Failed to create image view: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(msg);
		return nullptr;
	}

	return	_image_views.emplace_back(new ImageView(this, view)).get();
}
