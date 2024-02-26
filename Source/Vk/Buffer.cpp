#include "Buffer.hpp"
#include "Device.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"
using namespace Warp::Gpu;

Buffer::Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags flags, VmaAllocationCreateFlags memory_flags,
	VmaMemoryUsage memory_usage, VkDeviceSize alignment):
	_mapped_ptr(nullptr)
{
	const VkBufferCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = size,
		.usage = flags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
	};
	const VmaAllocationCreateInfo vci = {
		.flags = memory_flags,
		.usage = memory_usage
	};

	VkResult res;
	if (alignment == 0) {
		res = vmaCreateBufferWithAlignment(_device->get_allocator(), &ci, &vci, alignment, &_buffer, &_allocation, nullptr);
	} else {
		res = vmaCreateBuffer(_device->get_allocator(), &ci, &vci, &_buffer, &_allocation, nullptr);
	}

	if(res != VK_SUCCESS){
		MString msg = MString::format("Failed to create buffer: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(msg);
		throw Exception{ msg, res };
	}
}

Buffer::~Buffer()
{
	unmap();

	if(_buffer != VK_NULL_HANDLE)
		vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
}

void* Buffer::map()
{
	if(!_mapped_ptr){
		vmaMapMemory(_device->get_allocator(), _allocation, &_mapped_ptr);
	}
	return _mapped_ptr;
}

void Buffer::unmap()
{
	if(_mapped_ptr){
		vmaUnmapMemory(_device->get_allocator(), _allocation);
		_mapped_ptr = nullptr;
	}
}
