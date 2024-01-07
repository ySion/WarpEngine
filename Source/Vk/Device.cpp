#include "Device.hpp"

#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp;

Gpu::Device::Device(PhysicalDevice* physical_device, const MVector<const char*>& device_extension) {


	auto pproperties = physical_device->get_properties2();
	auto queue_properties = physical_device->get_queue_family_properties();

	// check if physical device support queue family support 4 type:
	// graphic && transfer && compute && present
	// is queue family 0 support all type
	if (!physical_device->is_queue_family0_support_all_queue()) {
		MString str = MString::format("Physical device {} queue family 0 support all [graphic && compute && transfer]", pproperties.properties.deviceName);
		error(str);
		throw Exception{ str, 0 };
	}
	// check if physical device support some features
	auto features = physical_device->get_features2();
	auto features_raytracing = physical_device->get_ray_tracing_features();
	auto features_acceleration_structure = physical_device->get_acceleration_structure_features();
	auto features_buffer_device_address = physical_device->get_buffer_device_address_features();
	auto features_descriptor_indexing = physical_device->get_descriptor_indexing_features();


	float queue_priorities[1] = { 1.0f };
	VkDeviceQueueCreateInfo queue_ci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = 0,
		.queueCount = 1,
		.pQueuePriorities = queue_priorities
	};

	//enable all features
	VkDeviceCreateInfo device_ci{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &features_raytracing,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_ci,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = to_u32(device_extension.size()),
		.ppEnabledExtensionNames = device_extension.data(),
		.pEnabledFeatures = &features.features
	};

	//VkDevice

	
	if (VkResult res = vkCreateDevice(*physical_device, &device_ci, nullptr, &_device);  res != VK_SUCCESS) {
		MString mstring = MString::format("Failed to create device: {} from {}.", static_cast<int>(res), pproperties.properties.deviceName);
		error(mstring);
		throw Exception{ mstring, res };
	} else {
		info("Vulkan device created");
	}

	VkQueue queue{};

	vkGetDeviceQueue(_device, 0, 0, &queue);

	_queue.reset(new Queue(queue));

	VmaVulkanFunctions vma_vulkan_func{};
	vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
	vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
	vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
	vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
	vma_vulkan_func.vkCreateImage = vkCreateImage;
	vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
	vma_vulkan_func.vkDestroyImage = vkDestroyImage;
	vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vma_vulkan_func.vkFreeMemory = vkFreeMemory;
	vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
	vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vma_vulkan_func.vkMapMemory = vkMapMemory;
	vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
	vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;
	vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
	vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
	vma_vulkan_func.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
	vma_vulkan_func.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
	vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
	vma_vulkan_func.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
	vma_vulkan_func.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;

	VmaAllocatorCreateInfo vma_ci{};
	vma_ci.device = _device;
	vma_ci.physicalDevice = *physical_device;
	vma_ci.instance = *physical_device->get_instance();
	vma_ci.vulkanApiVersion = VK_API_VERSION_1_3;
	vma_ci.pVulkanFunctions = &vma_vulkan_func;

	if (VkResult res = vmaCreateAllocator(&vma_ci, &_allocator); res != VK_SUCCESS) {
		MString mstring = MString::format("Failed to create VMA allocator: {}, {}.", (int)res, msg_map_VkResult(res));
		error(mstring);
		throw Exception{ mstring, res };
	}else
	{
		info("VMA allocator created");
	}
}
