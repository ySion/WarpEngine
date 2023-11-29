#pragma once

#include "GPUResourceBase.h"

namespace Warp {

	namespace GPU {

		class GPUCommandPool final : public GPUResource {
		public:
			inline GPUCommandPool(const MString& name) : GPUResource("command_pool", name) {}

			inline ~GPUCommandPool() override {
				vkDestroyCommandPool(GPUFactory::get_device(), m_command_pool, nullptr);
			}

			VkCommandPool m_command_pool{};
			uint32_t m_queue_family_index{};
			VkCommandPoolCreateFlags m_flag{};
		};
	}
}