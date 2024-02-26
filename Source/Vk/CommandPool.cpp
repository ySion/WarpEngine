#include "CommandPool.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp;

Gpu::CommandPool::CommandPool(Device* device): _device(device)
{
	const VkCommandPoolCreateInfo cmd_ci {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = 0
	};

	vkCreateCommandPool(*_device, &cmd_ci, nullptr, &_command_pool);
}

Gpu::CommandPool::~CommandPool()
{
	vkDestroyCommandPool(*_device, _command_pool, nullptr);
}

Gpu::CommandBuffer* Gpu::CommandPool::allocate(VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo cb_ci{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = _command_pool,
		.level = level,
		.commandBufferCount = 1
	};

	VkCommandBuffer buffer{};

	if (const auto res = vkAllocateCommandBuffers(*_device, &cb_ci, &buffer); res != VK_SUCCESS) {
		MString message = MString::format("Allocator Command Buffer Faield, code: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(message);
		throw Exception{ message, res };
	}

	return _cmd_buffers.emplace_back(new CommandBuffer(buffer, this)).get();
}
