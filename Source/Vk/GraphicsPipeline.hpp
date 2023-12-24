#pragma once

#include "VkBase.hpp"
#include "GraphicsPipelineState.hpp"
#include "PipelineLayout.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, GraphicsPipeline)

namespace Warp::Gpu {

	class GraphicsPipeline : public Inherit<GraphicsPipeline, Object> {
	public:
		GraphicsPipeline(Device* device, GraphicsPipelineState * state, RenderPass * renderpass, PipelineLayout * layout);

		~GraphicsPipeline() override;

		inline Device* get_device() const { return _device; }

		inline VkPipeline vk() const { return _pipeline; }

		operator VkPipeline() const { return _pipeline; }

	private:
		VkPipeline _pipeline {};

		Device* _device{};
	};
}