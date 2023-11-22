#pragma once

#include "Graphics/GPUResourceBase.h"

namespace Warp {
	namespace GPU {

		class GPUPipelineLayout final : public GPUResource {
		public:
			inline GPUPipelineLayout(const MString& name) : GPUResource("pipelinelayout", name) {}

			inline ~GPUPipelineLayout() override {
				vkDestroyPipelineLayout(GPUFactory::get_device(), m_layout, nullptr);
			}
			VkPipelineLayout m_layout{};
		};
	}
}
