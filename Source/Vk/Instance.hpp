#pragma once 
#include "Vk/VkBase.hpp"
#include "Core/Inherit.hpp"
#include "Core/TypeName.hpp"

WARP_TYPE_NAME_2(Gpu, Instance)
WARP_TYPE_NAME_2(Gpu, PhysicalDevice)

namespace Warp::Gpu {

	using Names = std::vector<const char*>;

	class PhysicalDevice : public Inherit<PhysicalDevice, Object> {
	public:

		using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;

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

		bool is_queue_family0_support_all_queue() const;

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

		using PhysicalDevices = std::vector<std::unique_ptr<PhysicalDevice>>;

		Instance(const std::vector<const char*>& extensions = {}, const std::vector<const char*>& layers = {});

		inline ~Instance() override { if (_instance) vkDestroyInstance(_instance, nullptr); }

		inline const PhysicalDevices& get_physical_devices() const { return _physicalDevices; }

		inline VkInstance vk() const { return _instance; }

		inline operator VkInstance() const { return _instance; }

		static bool is_extension_support(const char* extension_name);

		static bool is_layer_support(const char* extension_name);

	protected:

		VkInstance _instance{};

		PhysicalDevices _physicalDevices{};
	};

}