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

	const auto res_group = m_render_task_graph->get_render_system()->get_render_resource_group();

	auto frame_buffer_builder = res_group->get_frame_buffer_manager()->builder();
	auto frame_size = m_render_task_graph->get_static_resolution();

	const auto rednerpass_mag = res_group->get_render_pass_manager();
	m_render_pass = rednerpass_mag->find_by_name("test_task_node_1");
	if (!m_render_pass) {
		auto render_pass_builder = rednerpass_mag->builder("test_task_node_1");
		m_render_pass = render_pass_builder.add_attachment(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL).add_subpass({
				.color_attachment_refs = {0},
				.input_attachment_refs = {},
				.resolve_attachment_refs = {},
				.depth_stencil_resolve_attachment_ref = {},
				.preserve_attachment_refs = {},
				.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS
			}).make(true);
		if(!m_render_pass){
			return false;
		}
	}

	m_frame_buffer.reset(frame_buffer_builder.add_attachment(m_ref_images[0]).set_extent(frame_size).set_render_pass(m_render_pass).make(true));

	if(!m_frame_buffer){
		return false;
	}

	return true;
}

void Warp::Render::TaskNode_1::generate_commands(GPU::GPUCommandBuffer* commandbuf)
{
	VkExtent2D extent = m_render_task_graph->get_static_resolution();
	VkRect2D render_area{
		.offset = {0,0},
		.extent = {extent.width, extent.height}
	};
	VkClearValue clear_value[] = {
		{
			.color = {0.0f, 0.0f, 0.0f, 1.0f}
		}
	};

	commandbuf->begin_render_pass(m_render_pass, m_frame_buffer.get(), render_area, clear_value);

	VkViewport view_ports[] = { {
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(extent.width),
		.height = static_cast<float>(extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	} };
	commandbuf->set_viewports(view_ports);

	VkRect2D scissors[] = {{
		.offset = {0,0},
		.extent = {extent.width, extent.height}
	} };
	
	commandbuf->set_scissors(scissors);



	commandbuf->end_render_pass();

}
