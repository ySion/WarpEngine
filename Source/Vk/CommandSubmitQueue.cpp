#include "CommandSubmitQueue.hpp"
using namespace Warp;

Gpu::CommandSubmitQueue::CommandSubmitQueue(Device* device, uint32_t commandBufferCount)
{
	_commandPool = std::make_unique<CommandPool>(device);

	_queue = device->get_queue();

	_command_buffers.resize(commandBufferCount);
	for (int i = 0; i < commandBufferCount; ++i) {
		_command_buffers[i] = _commandPool->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}

	_current_command_buffer_index = 0;

	_max_command_buffer = to_u8(commandBufferCount);
}
