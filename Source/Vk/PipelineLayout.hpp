#pragma once

#include "Core/Inherit.hpp"

#include "VkBase.hpp"

#include "DescriptorSetLayout.hpp"

WARP_TYPE_NAME_2(Gpu, PipelineLayout)

namespace Warp::Gpu {

	class Device;

	class PipelineLayout : public Inherit<PipelineLayout, Object> {
	public:
		PipelineLayout(Device* device);

		inline PipelineLayout& add_set_layout(const DescriptorSetLayout* setLayout) {
			_setLayouts.push_back(*setLayout);
			return *this;
		}

		inline PipelineLayout& add_push_constant_range(VkPushConstantRange pushConstantRange) {
			_pushConstantRanges.push_back(pushConstantRange);
			return *this;
		}

		VkResult compile();

		~PipelineLayout() override;

		inline Device* get_device() const { return _device; }

		inline VkPipelineLayout vk() const { return _pipelineLayout; }

		operator VkPipelineLayout() const { return _pipelineLayout; }

	private:
		Device* _device;
		VkPipelineLayout _pipelineLayout {};
		MVector<VkDescriptorSetLayout> _setLayouts;
		MVector<VkPushConstantRange> _pushConstantRanges;
	};
}