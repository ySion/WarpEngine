#pragma once

#include "GPUResourceBase.h"

#include "GPUGraphicsPipeline.h"

#include "Graphics/GPUResourceHandle.h"

namespace Warp {
	namespace GPU {
		class GPURenderPass : public GPUResource {
		public:
			inline GPURenderPass(const MString& name) : GPUResource("render_pass", name) {}

			inline ~GPURenderPass() override {
				vkDestroyRenderPass(GPUFactory::get_device(), m_render_pass, nullptr);
			}

			VkRenderPass m_render_pass{};
			MVector<VkAttachmentDescription> m_attachment_descriptions{};
			MVector<GPUResourceHandle<GPUGraphicsPipeline>> m_pipelines{};
		};
	}
}
