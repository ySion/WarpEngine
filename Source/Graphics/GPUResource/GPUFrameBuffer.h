#pragma once

#include "Graphics/GPUFactory.h"
#include "Graphics/GPUResourceBase.h"

namespace Warp {
	namespace GPU {
		class GPUFrameBuffer : public GPUResource {
		public:
			inline GPUFrameBuffer(const MString& name) : GPUResource("framebuffer", name){}

			inline ~GPUFrameBuffer() override {
				vkDestroyFramebuffer(GPUFactory::get_device(), m_framebuffer, nullptr);
			}

			VkFramebuffer m_framebuffer{};
			VkExtent2D m_extent{};
			uint32_t m_layers;
			MVector<VkImageView> m_attachments_view{};
		};
	}
}
