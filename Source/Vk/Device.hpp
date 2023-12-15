#pragma once

#include "Core/Inherit.hpp"

#include "Vk/VkBase.hpp"
#include "Instance.hpp"

namespace Warp::Gpu
{
	using Names = MVector<const char*>;

	class Device : public Inherit<Device, Object> {
	public:

		
		Device(PhysicalDevice* physical_device, const Names& device_extension);

		~Device() override = default;


		inline Instance* get_instance() const { return _physical_device->get_instance(); }

		inline PhysicalDevice* get_physical_device() const{ return _physical_device; }


	private:

		PhysicalDevice* _physical_device{};

		VkDevice _device{};
	};
} 