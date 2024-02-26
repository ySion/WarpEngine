#pragma once

#include "RenderStage.hpp"

#include "Core/Inherit.hpp"
#include "Vk/Context.hpp"


WARP_TYPE_NAME_2(Gpu, RenderStage);

namespace Warp::Gpu {


	struct RenderStageCreateInfo {


	};

	//RenderNode, one RenderStage = one Renderpass
	class RenderStage : public Inherit<RenderStage, Object> {
	public:
		RenderStage(Context * ctx, RenderStageCreateInfo * ci) : _ctx(ctx)
		{
			/*_render_pass = std::make_unique<Gpu::RenderPass>(ctx->get_device());
			_render_pass->add_color_attachement(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
				.compile();*/
		}

		inline Gpu::Context * get_context() const noexcept { return _ctx; }

		//VkResult compile()

	private:
		std::unique_ptr<Gpu::RenderPass> _render_pass{};
		//MaterialInstance _material;
		Gpu::Context* _ctx{};
	};
};