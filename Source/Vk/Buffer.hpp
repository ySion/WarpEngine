#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, Buffer);

namespace Warp::Gpu {

	class Device;

	class Buffer : public Inherit<Buffer, Object> {
	public:
		Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags flags, VmaAllocationCreateFlags memory_flags = 0, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO, VkDeviceSize alignment = 0);

		~Buffer() override;

		void* map();

		void unmap();

	private:
		VkBuffer _buffer {};
		VmaAllocation _allocation {};
		Device* _device {};
		void* _mapped_ptr{};
	};
}