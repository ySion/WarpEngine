#include "RenderSystem.h"
#include "RenderTaskGraphDomain.h"

Warp::Render::RenderSystem::RenderSystem()
{
	m_render_resource_group = std::make_unique<RenderResourceGroup>();

	const auto ptr = m_render_resource_group->get_command_pool_manager()->builder("core_command_pool").set_configs(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT).make(true);
	for (int i = 0; i < 3; i++) {
		ptr->make_command_buffer();
	}
}

Warp::Render::RenderTaskGraphDomain* Warp::Render::RenderSystem::make_render_task_graph_domain(const MString& name, VkExtent2D resolution)
{
	if (m_render_task_graph_domains.contains(name)) {
		LOGE("[RenderSystem] On Make RenderTaskGraphDomain, \"{}\" already exist", name);
		return nullptr;
	}

	auto temp_ptr = std::unique_ptr<RenderTaskGraphDomain>(new RenderTaskGraphDomain(name, this, resolution));
	const auto temp_weak_ptr = temp_ptr.get();
	m_render_task_graph_domains[name] = std::move(temp_ptr);
	return temp_weak_ptr;
}

void Warp::Render::RenderSystem::remove_render_task_graph_domain(const MString& name)
{
	if (!m_render_task_graph_domains.contains(name)) {
		LOGE("[RenderSystem] On Remove RenderTaskGraphDomain, \"{}\" not exist", name);
		return;
	}

	m_render_task_graph_domains.erase(name);
}