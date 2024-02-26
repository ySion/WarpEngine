#include "Context.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"
#include "Core/observer_ptr.hpp"

using namespace Warp::Gpu;

Context::Context() {
	MVector<const char*> instance_extensions{
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	MVector<const char*> instance_layers{
		"VK_LAYER_KHRONOS_validation"
	};

	_instance = std::make_unique<Instance>(instance_extensions, instance_layers);

	const auto& physical_device_ref = _instance->get_physical_devices();

	observer_ptr<PhysicalDevice> physical_device_ptr = nullptr;
	for (auto& physical_device : physical_device_ref) {
		if (physical_device->is_queue_family0_support_all_queue()) {
			physical_device_ptr = physical_device.get();
			break;
		}
	}

	if (!physical_device_ptr.is_object_vaild()) {
		MString error_msg = "No physical device support all queue family";
		error(error_msg);
		throw Exception{ error_msg, 0 };
	}

	MVector device_extension_names = {
		"VK_KHR_swapchain",
		"VK_KHR_get_memory_requirements2",
		"VK_KHR_dedicated_allocation",
		"VK_KHR_bind_memory2",
		"VK_KHR_spirv_1_4"
	};

	_device = std::make_unique<Device>(physical_device_ptr.get(), device_extension_names);

	//3重缓冲
	_command_submit_queue = std::make_unique<CommandSubmitQueue>(_device.get(), 3);
}

std::unique_ptr<Window> Context::create_window(const MString& title, int w, int h, uint32_t flag) {
	return std::make_unique<Window>(this, title, w, h, flag);
}

std::unique_ptr<Image> Context::create_image(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage,
	VmaAllocationCreateFlags memory_flags, VkImageType imageType, uint32_t mip_levels, uint32_t arrayLayers,
	VkSampleCountFlagBits samples, VmaMemoryUsage memory_usage, VkImageTiling tiling, VkImageLayout layout) const {
	try {
		return std::make_unique<Image>(_device.get(), format, extent, usage, memory_flags, imageType, mip_levels, arrayLayers, samples, memory_usage, tiling, layout);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<Image> Context::create_image_depth(VkExtent3D extent, VmaAllocationCreateFlags memory_flags,
	VkImageUsageFlags usage, uint32_t mip_levels, uint32_t arrayLayers, VkSampleCountFlagBits samples,
	VmaMemoryUsage memory_usage, VkImageTiling tiling, VkImageLayout layout) const {
	try {
		auto img = std::make_unique<Image>(_device.get(), VK_FORMAT_D32_SFLOAT, extent, usage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						memory_flags, VK_IMAGE_TYPE_2D, mip_levels, arrayLayers, samples, memory_usage, tiling, layout);
		img->create_view(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT, { VK_COMPONENT_SWIZZLE_IDENTITY }, VK_IMAGE_ASPECT_DEPTH_BIT);
		return img;
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<Image> Context::create_image_color(VkExtent3D extent, VmaAllocationCreateFlags memory_flags,
	VkImageUsageFlags usage, uint32_t mip_levels, uint32_t arrayLayers, VkSampleCountFlagBits samples,
	VmaMemoryUsage memory_usage, VkImageTiling tiling, VkImageLayout layout) const {
	try {
		auto img = std::make_unique<Image>(_device.get(), VK_FORMAT_R8G8B8A8_SRGB, extent, usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
									memory_flags, VK_IMAGE_TYPE_2D, mip_levels, arrayLayers, samples, memory_usage, tiling, layout);
		img->create_view(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, { VK_COMPONENT_SWIZZLE_IDENTITY }, VK_IMAGE_ASPECT_COLOR_BIT);
		return img;
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<Buffer> Context::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
	VmaAllocationCreateFlags memory_flags, VmaMemoryUsage memory_usage, VkDeviceSize alignment) const
{
	try {
		return std::make_unique<Buffer>(_device.get(), size, usage, memory_flags, memory_usage, alignment);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<DescriptorPool> Context::create_descriptor_pool(uint32_t maxSets) const {
	try {
		return std::make_unique<DescriptorPool>(_device.get(), maxSets);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<DescriptorSetLayout> Context::create_descriptor_set_layout(VkDescriptorBindingFlags flags) const {
	try {
		return std::make_unique<DescriptorSetLayout>(_device.get(), flags);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<RenderPass> Context::create_render_pass() const {
	try {
		return std::make_unique<RenderPass>(_device.get());
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<GraphicsPipeline> Context::create_graphics_pipeline(GraphicsPipelineState* state, RenderPass* renderpass, PipelineLayout* layout) const {
	try {
		return std::make_unique<GraphicsPipeline>(_device.get(), state, renderpass, layout);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<ShaderModule> Context::create_shader_module(ShaderType shader_type, const MVector<uint32_t>& spirv) const {
	try {
		return std::make_unique<ShaderModule>(_device.get(), shader_type, spirv);
	} catch (const Exception& e) {
		return nullptr;
	}
}

std::unique_ptr<PipelineLayout> Context::create_pipeline_layout() const {
	try {
		return std::make_unique<PipelineLayout>(_device.get());
	} catch (const Exception& e) {
		return nullptr;
	}
}
