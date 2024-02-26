#pragma once 
#include "Vk/VkBase.hpp"
#include "Core/Inherit.hpp"
#include "Core/TypeName.hpp"

WARP_TYPE_NAME_2(Gpu, Instance)
WARP_TYPE_NAME_2(Gpu, PhysicalDevice)

namespace Warp::Gpu {

	using Names = MVector<const char*>;

	class PhysicalDevice : public Inherit<PhysicalDevice, Object> {
	public:

		using QueueFamilyProperties = MVector<VkQueueFamilyProperties>;

		PhysicalDevice(VkPhysicalDevice physicalDevice, Instance* instance);

		inline ~PhysicalDevice() override = default;

		inline VkPhysicalDevice vk() const { return _physicalDevice; }

		inline Instance* get_instance() const { return _instance; }

		inline operator VkPhysicalDevice() const { return _physicalDevice; }

		inline QueueFamilyProperties& get_queue_family_properties() { return _queueFamilyProperties; }

		inline VkPhysicalDeviceFeatures2& get_features2() { return _features2; }

		inline VkPhysicalDeviceProperties2& get_properties2() { return _properties2; }

		inline VkPhysicalDeviceMemoryProperties2& get_memory_properties2() { return _memoryProperties2; }

		inline VkPhysicalDeviceRayTracingPipelinePropertiesKHR& get_ray_tracing_pipeline_properties() { return _rayTracingPipelineProperties; }

		inline VkPhysicalDeviceAccelerationStructurePropertiesKHR& get_acceleration_structure_properties() { return _accelerationStructureProperties; }

		inline VkPhysicalDeviceRayTracingPipelineFeaturesKHR& get_ray_tracing_features() { return _rayTracingFeatures; }

		inline VkPhysicalDeviceAccelerationStructureFeaturesKHR& get_acceleration_structure_features() { return _accelerationStructureFeatures; }

		inline VkPhysicalDeviceBufferDeviceAddressFeatures& get_buffer_device_address_features() { return _bufferDeviceAddressFeatures; }

		inline VkPhysicalDeviceDescriptorIndexingFeatures& get_descriptor_indexing_features() { return _descriptorIndexingFeatures; }

		inline VkPhysicalDeviceDescriptorIndexingProperties& get_descriptor_indexing_properties() { return _descriptorIndexingProperties; }

		inline VkPhysicalDeviceVulkan11Features& get_vulkan11_features() { return _vulkan11Features; }

		inline VkPhysicalDeviceVulkan11Properties& get_vulkan11_properties() { return _vulkan11Properties; }

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

		VkPhysicalDeviceVulkan11Features _vulkan11Features{};

		VkPhysicalDeviceVulkan11Properties _vulkan11Properties{};
	};

	class Instance : public Inherit<Instance, Object> {
	public:

		using PhysicalDevices = MVector<std::unique_ptr<PhysicalDevice>>;

		Instance(const MVector<const char*>& extensions = {}, const MVector<const char*>& layers = {});

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