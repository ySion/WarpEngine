#include "RenderPass.hpp"
#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

using namespace Warp::Gpu;

RenderPass::RenderPass(Device* device) : _device{ device }
{}

RenderPass::~RenderPass() {
	if (_render_pass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(*_device, _render_pass, nullptr);
	}
}

VkResult RenderPass::compile()
{
	MVector<VkAttachmentDescription> attachments{ _input_att.size() + _color_att.size() + _resolve_att.size() + (_depth_att.has_value() ? 1 : 0) };

	MVector<VkAttachmentReference> input_ref{ _input_att.size() };
	MVector<VkAttachmentReference> color_ref{ _color_att.size() };
	MVector<VkAttachmentReference> resolve_ref{ _resolve_att.size() };
	VkAttachmentReference depth_ref{};

	uint32_t att_idx = 0;

	for (size_t i = 0; i < _input_att.size(); ++i) {
		auto& ctx = _input_att[i];
		attachments[att_idx] = {
			.flags = 0,
			.format = ctx.format,
			.samples = ctx.samples,
			.loadOp = ctx.load_op,
			.storeOp = ctx.store_op,
			.stencilLoadOp = ctx.stencil_load_op,
			.stencilStoreOp = ctx.stencil_store_op,
			.initialLayout = ctx.initial_layout,
			.finalLayout = ctx.final_layout
		};

		input_ref[i] = {
			.attachment = att_idx,
			.layout = ctx.ref_layout
		};

		att_idx++;
	}


	for (size_t i = 0; i < _color_att.size(); ++i) {
		auto& ctx = _color_att[i];
		attachments[att_idx] = {
			.flags = 0,
			.format = ctx.format,
			.samples = ctx.samples,
			.loadOp = ctx.load_op,
			.storeOp = ctx.store_op,
			.stencilLoadOp = ctx.stencil_load_op,
			.stencilStoreOp = ctx.stencil_store_op,
			.initialLayout = ctx.initial_layout,
			.finalLayout = ctx.final_layout
		};

		color_ref[i] = {
			.attachment = att_idx,
			.layout = ctx.ref_layout
		};

		att_idx++;
	}

	if (_depth_att.has_value()) {
		auto& val = _depth_att.value();
		attachments[att_idx] = {
			.flags = 0,
			.format = val.format,
			.samples = val.samples,
			.loadOp = val.load_op,
			.storeOp = val.store_op,
			.stencilLoadOp = val.stencil_load_op,
			.stencilStoreOp = val.stencil_store_op,
			.initialLayout = val.initial_layout,
			.finalLayout = val.final_layout
		};

		depth_ref = VkAttachmentReference{
			.attachment = att_idx,
			.layout = val.ref_layout
		};
		att_idx++;
	}


	for (size_t i = 0; i < _resolve_att.size(); ++i) {
		auto& ctx = _resolve_att[i];
		attachments[att_idx] = {
			.flags = 0,
			.format = ctx.format,
			.samples = ctx.samples,
			.loadOp = ctx.load_op,
			.storeOp = ctx.store_op,
			.stencilLoadOp = ctx.stencil_load_op,
			.stencilStoreOp = ctx.stencil_store_op,
			.initialLayout = ctx.initial_layout,
			.finalLayout = ctx.final_layout
		};

		resolve_ref[i] = {
			.attachment = att_idx,
			.layout = ctx.ref_layout
		};

		att_idx++;
	}

	VkSubpassDescription sbp_desc = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = to_u32(input_ref.size()),
		.pInputAttachments = input_ref.data(),
		.colorAttachmentCount = to_u32(color_ref.size()),
		.pColorAttachments = color_ref.data(),
		.pResolveAttachments = resolve_ref.data(),
		.pDepthStencilAttachment = _depth_att.has_value() ? &depth_ref : nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};

	VkSubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	VkRenderPassCreateInfo rp_ci = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = to_u32(attachments.size()),
		.pAttachments = attachments.data(),
		.subpassCount = 1,
		.pSubpasses = &sbp_desc,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	if (VkResult r = vkCreateRenderPass(*_device, &rp_ci, nullptr, &_render_pass); r != VK_SUCCESS) {
		MString msg = MString::format("Failed to create render pass: {}, {}.", static_cast<int>(r) , msg_map_VkResult(r));
		error(msg);
		return r;
	}
	info("Render Pass Create Success");
	return VK_SUCCESS;
}
