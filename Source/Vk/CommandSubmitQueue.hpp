#pragma once

#include "Core/Inherit.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"

WARP_TYPE_NAME_2(Gpu, CommandSubmitQueue)

namespace Warp::Gpu {

	class CommandSubmitQueue : public Inherit<CommandSubmitQueue, Object> {
	public:
		CommandSubmitQueue(Device* device, uint32_t commandBufferCount);


		inline CommandBuffer* get_current_command_buffer() const {
			return _command_buffers[_current_command_buffer_index];
		}

		inline void submit() const {
			
		}

		inline void next_command_buffer() {
			_current_command_buffer_index = (_current_command_buffer_index + 1) % _max_command_buffer;
		}

		inline Device* get_device() const { return _device; }

	private:
		std::unique_ptr<CommandPool> _commandPool {};

		MVector<CommandBuffer*> _command_buffers {};

		uint8_t _current_command_buffer_index {};

		uint8_t _max_command_buffer {};

		Queue* _queue{};

		Device* _device{};
	};
}