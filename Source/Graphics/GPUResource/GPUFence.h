#pragma once

#include "GPUResourceBase.h"

#include <algorithm>

namespace Warp {

	namespace GPU {

		class GPUFence : public GPUResource {
		public:
			inline GPUFence(const MString& name) : GPUResource("fence", name) {}

			inline ~GPUFence() override {
				vkDestroyFence(GPUFactory::get_device(), m_fence, nullptr);
			}

			void reset_signal() const {
				vkResetFences(GPUFactory::get_device(), 1, &m_fence);
			}

			VkFence m_fence{};
		};

		inline void wait_for_fences(const MVector<VkFence>& fences) {
			vkWaitForFences(GPUFactory::get_device(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
		}

		inline void reset_fences(const MVector<VkFence>& fences) {
			vkResetFences(GPUFactory::get_device(), fences.size(), fences.data());
		}

		inline void wait_for_fences(const MVector<GPUFence*>& fences) {
			MVector<VkFence> res{ fences.size() };
			std::ranges::transform(fences, res.begin(), [](const GPUFence* fence) { return fence->m_fence; });
			wait_for_fences(res);
		}

		inline void reset_fences(const MVector<GPUFence*>& fences) {
			MVector<VkFence> res{ fences .size()};
			std::ranges::transform(fences, res.begin(),[](const GPUFence* fence) { return fence->m_fence; });
			reset_fences(res);
		}
	}
}
