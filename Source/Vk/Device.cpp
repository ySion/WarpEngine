#include "Device.hpp"

#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp;

Gpu::Device::Device(PhysicalDevice* physical_device, const Names& device_extension)
{

	//check if physical device support queue family support 4 type: graphic transfer compute present

	auto pproperties = physical_device->get_properties2();
	auto queue_properties = physical_device->get_queue_family_properties();

	// check if queue family 0 support all type
	if (!physical_device->is_queue_family0_support_all_queue()) {
	} else {
		error("Physical device {} queue family 0 dont't support all [graphic && compute && transfer]", pproperties.properties.deviceName);
		throw Exception{ "Create Device faield : Physical queue family 0 dont't support all [graphic && compute && transfer]", 0 };
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
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_ci,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = device_extension.size_u32(),
		.ppEnabledExtensionNames = device_extension.data(),
		.pEnabledFeatures = &features.features
	};
}
