#pragma once

#include "Core/Inherit.hpp"
#include "RenderStage.hpp"

#include "Vk/Context.hpp"

WARP_TYPE_NAME_2(Gpu, Graphics)

namespace Warp::Gpu {
	class RenderGraph : public Inherit<RenderGraph, Object> {
	public:
		RenderGraph() = default;
		~RenderGraph() override = default;

	private:
		MVector<std::unique_ptr<RenderStageCreateInfo>> _stages_ci {};

	private:
		MVector<std::unique_ptr<RenderStage>> _stages[3]{};
		MVector<std::unique_ptr<Gpu::Buffer>> _buffers[3]{};
		MVector<std::unique_ptr<Gpu::Image>> _images[3]{};
	};

}