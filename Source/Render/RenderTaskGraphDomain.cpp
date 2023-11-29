#include "RenderTaskGraphDomain.h"

#include "RenderSystem.h"

Warp::Render::RenderTaskGraphDomain::RenderTaskGraphDomain(const MString& name, RenderSystem* render_system,
                                                           VkExtent2D static_resolution): m_name(name), m_resource_static_resolution(static_resolution), m_render_system(render_system)
{
	m_render_task_graphs.reserve(3);

	for (int i = 0; i < 3; ++i) {
		MString temp_name = mformat("{}_{}", name, i);
		auto temp_ptr = std::unique_ptr<RenderTaskGraph>(new RenderTaskGraph(temp_name, render_system, static_resolution, i));
		m_render_task_graphs.emplace_back(std::move(temp_ptr));
	}

	for(int i = 0; i < 3; ++i) {
		m_render_task_graphs[i]->set_previous_frame_graph(m_render_task_graphs[(i + 1) % 3].get());
	}
}

Warp::Render::RenderTaskGraphDomain& Warp::Render::RenderTaskGraphDomain::add_image_node(const MString& name,
                                                                                         const MString& type) {
	for (const auto& i : m_render_task_graphs) {
		i->add_image_node(name, type);
	}
	return *this;
}

Warp::Render::RenderTaskGraphDomain& Warp::Render::RenderTaskGraphDomain::add_buffer_node(const MString& name,
	const MString& type, uint64_t size) {

	for (const auto& i : m_render_task_graphs) {
		i->add_buffer_node(name, type, size);
	}
	return *this;
}

Warp::Render::RenderTaskGraphDomain& Warp::Render::RenderTaskGraphDomain::add_task_node(const MString& name,
	const MString& type, const MVector<uint32_t>& ref_images, const MVector<uint32_t>& ref_buffers) {
	for (const auto& i : m_render_task_graphs) {
		i->add_task_node(name, type, ref_images, ref_buffers);
	}
	return *this;
}

Warp::Render::RenderTaskGraphDomain& Warp::Render::RenderTaskGraphDomain::set_screen_resolution(
	const VkExtent2D& resolution) {
	for (const auto& i : m_render_task_graphs) {
		i->set_screen_resolution(resolution);
	}
	return *this;
}

void Warp::Render::RenderTaskGraphDomain::clear() const {
	for (const auto& i : m_render_task_graphs) {
		i->clear();
	}
}

bool Warp::Render::RenderTaskGraphDomain::compile() const {
	bool success = true;
	for (const auto& i : m_render_task_graphs) {
		success &= i->compile();
	}
	return success;
}

Warp::GPU::GPUImage* Warp::Render::RenderTaskGraphDomain::get_gpu_image(const MString& name, uint32_t frame_idx) const {
	MString find_name = mformat("{}_{}_{}", m_name, frame_idx, name);
	const auto ptr = m_render_system->get_render_resource_group()->get_image_manager()->find_by_name(find_name);
	return ptr;
}
