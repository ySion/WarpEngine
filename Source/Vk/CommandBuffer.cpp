#include "CommandBuffer.hpp"

#include "Core/Logger.hpp"

using namespace Warp;

Gpu::CommandBuffer::CommandBuffer(VkCommandBuffer cmdbuffer, CommandPool* parent_pool) : _command_buffer(cmdbuffer), _parent_pool(parent_pool) {
	info("Command buffer create");
}

Gpu::CommandBuffer::~CommandBuffer() = default;
