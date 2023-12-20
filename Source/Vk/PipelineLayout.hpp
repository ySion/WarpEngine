#pragma once

#include "Core/Inherit.hpp"

#include "VkBase.hpp"

WARP_TYPE_NAME_2(Gpu, PipelineLayout)

namespace Warp::Gpu {

	class Device;

	class PipelineLayout : public Inherit<PipelineLayout, Object> {
	public:
		PipelineLayout(Device* device, 
			uint32_t setLayoutCount,
			const VkDescriptorSetLayout* pSetLayouts,
			uint32_t pushConstantRangeCount = 0,
			const VkPushConstantRange* pPushConstantRanges = nullptr);

		inline VkPipelineLayout vk() const { return _pipelineLayout; }

		operator VkPipelineLayout() const { return _pipelineLayout; }

		~PipelineLayout() override;

	private:
		VkPipelineLayout _pipelineLayout {};
		Device* _device;
	};
}