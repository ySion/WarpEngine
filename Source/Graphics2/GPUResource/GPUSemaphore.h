#pragma once

#include "GPUResourceBase.h"

namespace Warp {
	namespace GPU {

		class GPUSemaphore final : public GPUResource {
		public:
			inline GPUSemaphore(const MString& name) : GPUResource("semaphore", name) {}

			inline ~GPUSemaphore() override {
				vkDestroySemaphore(GPUFactory::get_device(), m_semaphore, nullptr);
			}

			VkSemaphore m_semaphore{};
		};
	}
}
