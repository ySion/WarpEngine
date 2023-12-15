#pragma once 
#include "Vk/VkBase.hpp"
#include "Core/Inherit.hpp"
#include "Core/Exception.hpp"

namespace Warp::Gpu {

	class Instance;

	class PhysicalDevice : public Inherit<PhysicalDevice, Object> {
	public:

		using QueueFamilyProperties = MVector<VkQueueFamilyProperties>;

		PhysicalDevice(VkPhysicalDevice physicalDevice, Instance* instance);

		inline ~PhysicalDevice() override = default;

		inline VkPhysicalDevice vk() const { return _physicalDevice; }

		inline Instance* get_instance() const { return _instance; }

		inline operator VkPhysicalDevice() const { return _physicalDevice; }

		inline QueueFamilyProperties get_queue_family_properties() const { return _queueFamilyProperties; }

		inline VkPhysicalDeviceFeatures2 get_features2() const { return _features2; }

		inline VkPhysicalDeviceProperties2 get_properties2() const { return _properties2; }

		inline VkPhysicalDeviceMemoryProperties2 get_memory_properties2() const { return _memoryProperties2; }

		inline VkPhysicalDeviceRayTracingPipelinePropertiesKHR get_ray_tracing_pipeline_properties() const { return _rayTracingPipelineProperties; }

		inline VkPhysicalDeviceAccelerationStructurePropertiesKHR get_acceleration_structure_properties() const { return _accelerationStructureProperties; }

		inline VkPhysicalDeviceRayTracingPipelineFeaturesKHR get_ray_tracing_features() const { return _rayTracingFeatures; }

		inline VkPhysicalDeviceAccelerationStructureFeaturesKHR get_acceleration_structure_features() const { return _accelerationStructureFeatures; }

		inline VkPhysicalDeviceBufferDeviceAddressFeatures get_buffer_device_address_features() const { return _bufferDeviceAddressFeatures; }

		inline VkPhysicalDeviceDescriptorIndexingFeatures get_descriptor_indexing_features() const { return _descriptorIndexingFeatures; }

		inline VkPhysicalDeviceDescriptorIndexingProperties get_descriptor_indexing_properties() const { return _descriptorIndexingProperties; }

		inline bool is_queue_family0_support_all_queue() const {

			if (!_queueFamilyProperties.empty()) {
				return false;
			}

			// check if queue family 0 support all type
			if (_queueFamilyProperties[0].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
				_queueFamilyProperties[0].queueFlags & VK_QUEUE_COMPUTE_BIT &&
				_queueFamilyProperties[0].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			} else {
				return false;
			}

			return true;
		}

	private:
		Instance* _instance{};

		VkPhysicalDevice _physicalDevice{};

		QueueFamilyProperties _queueFamilyProperties;

		VkPhysicalDeviceFeatures2 _features2{};

		VkPhysicalDeviceProperties2 _properties2{};

		VkPhysicalDeviceMemoryProperties2 _memoryProperties2{};

		VkPhysicalDeviceRayTracingPipelinePropertiesKHR _rayTracingPipelineProperties{};

		VkPhysicalDeviceAccelerationStructurePropertiesKHR _accelerationStructureProperties{};

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR _rayTracingFeatures{};

		VkPhysicalDeviceAccelerationStructureFeaturesKHR _accelerationStructureFeatures{};

		VkPhysicalDeviceBufferDeviceAddressFeatures _bufferDeviceAddressFeatures{};

		VkPhysicalDeviceDescriptorIndexingFeatures _descriptorIndexingFeatures{};

		VkPhysicalDeviceDescriptorIndexingProperties _descriptorIndexingProperties{};
	};

	class Instance : public Inherit<Instance, Object> {
	public:

		using PhysicalDevices = MVector<own_ptr<PhysicalDevice>>;

		Instance(const MVector<const char*>& extensions = {}, const MVector<const char*>& layers = {});

		inline ~Instance() override { if (_instance) vkDestroyInstance(_instance, nullptr); }

		inline const PhysicalDevices& get_physical_devices() const { return _physicalDevices; }

		inline VkInstance vk() const { return _instance; }

		inline operator VkInstance() const { return _instance; }

		static bool is_extension_support(const char* extension_name);

		static bool is_layer_support(const char* extension_name);

	private:

		VkInstance _instance{};

		VkPhysicalDevice _physicalDevice{};

		PhysicalDevices _physicalDevices;

	};

}