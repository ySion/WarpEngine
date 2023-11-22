#pragma once

#include "Graphics/GPUFactory.h"
#include "Graphics/GPUResourceBase.h"

namespace Warp {
	namespace GPU {

		class GPURenderPass;
		class GPUGraphicsPipeline : public GPUResource {
		public:
			inline GPUGraphicsPipeline(const MString& name) : GPUResource("pipeline", name) {}

			inline ~GPUGraphicsPipeline() override {
				vkDestroyPipeline(GPUFactory::get_device(), m_pipeline, nullptr);
			}

			VkPipeline m_pipeline{};
			GPURenderPass* m_render_pass{};
			uint32_t m_subpass_idx{};
		};
	}
}
