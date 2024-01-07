#pragma once

#include "VkBase.hpp"

#include "Device.hpp"
#include "CommandBuffer.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, CommandPool)

namespace Warp::Gpu {


	class CommandPool : public Inherit<CommandPool, Object> {
	public:
		CommandPool(Device* device);

		~CommandPool() override;

		CommandBuffer* allocate(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		inline VkCommandPool vk_command_pool() const { return _command_pool; }

		operator VkCommandPool() const { return _command_pool; }

	private:

		VkCommandPool _command_pool{};

		Device* _device{};

		MVector<std::unique_ptr<CommandBuffer>> _cmd_buffers{};
	};

}