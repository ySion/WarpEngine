#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, CommandBuffer)

namespace Warp::Gpu {

	class CommandPool;

	class CommandBuffer :public Inherit<CommandBuffer, Object> {
	public:
		CommandBuffer(VkCommandBuffer cmdbuffer, CommandPool* parent_pool);

		~CommandBuffer() override;

		VkCommandBuffer vk() const { return _command_buffer; }

		operator VkCommandBuffer() const { return _command_buffer; }

	private:
		VkCommandBuffer _command_buffer{};
		CommandPool* _parent_pool{};
		friend class CommandPool;
	};
}