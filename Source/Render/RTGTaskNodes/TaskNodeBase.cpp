#include "TaskNodeBase.h"

#include "Render/RenderSystem.h"
#include "Render/RenderTaskGraph.h"

bool Warp::Render::TaskNode_1::prepare() {

	if (m_ref_images.size() != 1) {
		return false;
	}

	if (m_ref_images[0] == nullptr) {
		return false;
	}

	const auto res_group = m_render_system->get_render_system()->get_render_resource_group();

	auto frame_buffer_builder = res_group->get_frame_buffer_manager()->builder();
	auto frame_size = m_render_system->get_static_resolution();

	const auto rednerpass_mag = res_group->get_render_pass_manager();
	auto ptr = rednerpass_mag->find_by_name("test_task_node_1");
	if (!ptr) {
		auto render_pass_builder = rednerpass_mag->builder("test_task_node_1");
		ptr = render_pass_builder.add_attachment(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL).add_subpass({
				.color_attachment_refs = {0},
				.input_attachment_refs = {},
				.resolve_attachment_refs = {},
				.depth_stencil_resolve_attachment_ref = {},
				.preserve_attachment_refs = {},
				.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS
			}).make(true);
		if(!ptr){
			return false;
		}
	}

	const auto frame_ptr = frame_buffer_builder.add_attachment(m_ref_images[0]).set_extent(frame_size).set_render_pass(ptr).make(true);

	if(!frame_ptr){
		return false;
	}

	return true;
}
