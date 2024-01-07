#pragma once

#include "GPUResourceBase.h"

namespace Warp {
	namespace GPU {
		class GPUShader final : public GPUResource {
		public:

			inline  GPUShader(const MString& name) : GPUResource("shader", name) {}

			inline  ~GPUShader() override {
				vkDestroyShaderModule(GPUFactory::get_device(), m_module, nullptr);
			}

			EShLanguage m_stage{};
			MString m_code{};
			VkShaderModule m_module{};
			MVector<uint32_t> m_spirv{};
		};
	}
}