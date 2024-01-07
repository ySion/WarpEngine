#pragma once


#include "GPUResourceBase.h"

#include "Core/Log.h"

namespace Warp {

	namespace GPU {

		class GPUDescriptorSetLayout : public GPUResource {
		public:
			inline GPUDescriptorSetLayout(const MString& name) : GPUResource("descriptor_set_layout", name) {}

			inline ~GPUDescriptorSetLayout() {
				vkDestroyDescriptorSetLayout(GPUFactory::get_device(), m_layout, nullptr);
			}

			VkDescriptorSetLayout m_layout;
		};
	}
}