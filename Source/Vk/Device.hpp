#pragma once

#include "Core/Inherit.hpp"

#include "Vk/VkBase.hpp"
#include "Instance.hpp"
#include "Queue.hpp"

WARP_TYPE_NAME_2(Gpu, Device)

namespace Warp::Gpu {

	class Device : public Inherit<Device, Object> {
	public:

		Device(PhysicalDevice* physical_device, const MVector<const char*>& device_extension);

		inline ~Device() override {
			vmaDestroyAllocator(_allocator);
			if (_device) vkDestroyDevice(_device, nullptr);
		}

		inline Instance* get_instance() const { return _physical_device->get_instance(); }

		inline PhysicalDevice* get_physical_device() const { return _physical_device; }

		inline Queue* get_queue() const { return _queue.get(); }

		inline VmaAllocator get_allocator() const { return _allocator; }

		operator VkDevice() const { return _device; }

		VkDevice vk() const { return _device; }


	protected:

		PhysicalDevice* _physical_device{};

		VkDevice _device{};

		std::unique_ptr<Queue> _queue{};

		VmaAllocator _allocator{};
	};
}