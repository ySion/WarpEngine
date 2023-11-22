#pragma once
#include "Graphics/GPUResourceBase.h"


namespace Warp {
	namespace GPU {

		class GPUDescriptorPool final : public GPUResource {
		public:
			inline GPUDescriptorPool(const MString& name) : GPUResource("descriptorpool", name) {}

			inline ~GPUDescriptorPool() override {
				vkDestroyDescriptorPool(GPUFactory::get_device(), m_pool, nullptr);
			}

			VkDescriptorPool m_pool{};
		};
	}

}
