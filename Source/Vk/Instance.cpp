#include "Instance.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"
#include "glslang/Include/glslang_c_interface.h"

using namespace Warp;

Gpu::PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice, Instance* instance) {
	_instance = instance;

	if(!physicalDevice) {
		throw Exception{ "Physical Device is nullptr", 0 };
	}

	_physicalDevice = physicalDevice;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);
	_queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, _queueFamilyProperties.data());


	_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	_features2.pNext = &_rayTracingFeatures;



	_rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	_rayTracingFeatures.pNext = &_accelerationStructureFeatures;

	_accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	_accelerationStructureFeatures.pNext = &_bufferDeviceAddressFeatures;

	_bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	_bufferDeviceAddressFeatures.pNext = &_descriptorIndexingFeatures;

	_descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	_descriptorIndexingFeatures.pNext = &_vulkan11Features;

	_vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

	vkGetPhysicalDeviceFeatures2(_physicalDevice, &_features2);


	//Get Properties

	_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	_properties2.pNext = &_rayTracingPipelineProperties;

	_rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	_rayTracingPipelineProperties.pNext = &_accelerationStructureProperties;

	_accelerationStructureProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
	_accelerationStructureProperties.pNext = &_descriptorIndexingProperties;

	_descriptorIndexingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
	_descriptorIndexingProperties.pNext = &_vulkan11Properties;

	_vulkan11Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;

	_memoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

	vkGetPhysicalDeviceProperties2(_physicalDevice, &_properties2);

	vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &_memoryProperties2);
}

bool Gpu::PhysicalDevice::is_queue_family0_support_all_queue() const
{

	if (_queueFamilyProperties.empty()) {
		return false;
	}

	// check if queue family 0 support all type
	if ((_queueFamilyProperties[0].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
		(_queueFamilyProperties[0].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
		(_queueFamilyProperties[0].queueFlags & VK_QUEUE_TRANSFER_BIT)) {
	} else {
		return false;
	}

	return true;
}

Gpu::Instance::Instance(const MVector<const char*>& extensions, const MVector<const char*>& layers)
{
	volkInitialize();

	//Check Support

	for (auto& layer : layers) {
		if (!is_layer_support(layer)) {
			error("later not supported: {}", layer);
			throw Exception{ "Layer not supported", 0 };
		}
	}

	for(auto& extension : extensions) {
		if(!is_extension_support(extension)) {
			error("VKnstance Extension not supported: {}", extension);
			throw Exception{ "VKnstance Extension not supported", 0 };
		}
	}

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Warp Engine",
		.pEngineName = "Warp Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = to_u32(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = to_u32(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	};

	if (VkResult res = vkCreateInstance(&createInfo, nullptr, &_instance); res != VK_SUCCESS) {
		error("Failed to create Vulkan instance: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		throw Exception{ "Failed to create instance", res };
	}else {
		info("Vulkan instance created");
	}

	volkLoadInstance(_instance);
	uint32_t physical_device_count = 0;
	vkEnumeratePhysicalDevices(_instance,&physical_device_count, nullptr);
	MVector<VkPhysicalDevice> physicalDevices{physical_device_count};
	vkEnumeratePhysicalDevices(_instance, &physical_device_count, physicalDevices.data());

	for (const auto& physicalDevice : physicalDevices) {
		_physicalDevices.emplace_back(new PhysicalDevice(physicalDevice, this));
	}
}

bool Gpu::Instance::is_extension_support(const char* extension_name)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	MVector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	for (const auto& extension : extensions) {
		if (strcmp(extension.extensionName, extension_name) == 0){ return true; }
	}

	return false;
}

bool Gpu::Instance::is_layer_support(const char* extension_name)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	MVector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	for (const auto& layer : layers) {
		if (strcmp(layer.layerName, extension_name) == 0) { return true; }
	}

	return false;
}
