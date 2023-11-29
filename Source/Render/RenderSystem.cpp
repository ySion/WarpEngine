#include "RenderSystem.h"
#include "RenderTaskGraph.h"
#include "RenderTaskGraphDomain.h"

Warp::Render::RenderSystem::RenderSystem()
{
	m_render_resource_group = std::make_unique<RenderResourceGroup>();

	
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

Warp::Render::RenderCommandDomain* Warp::Render::RenderSystem::make_command_domain(const MString& name)
{
	if (m_command_domain.contains(name)) {
		LOGE("[RenderSystem] On Make RenderCommandDomain, \"{}\" already exist", name);
		return nullptr;
	}

	auto temp_ptr = std::unique_ptr<RenderCommandDomain>(new RenderCommandDomain(name, this));
	const auto temp_weak_ptr = temp_ptr.get();
	m_command_domain[name] = std::move(temp_ptr);
	return temp_weak_ptr;
}

void Warp::Render::RenderSystem::remove_command_domain(const MString& name)
{
	if (!m_command_domain.contains(name)) {
		LOGE("[RenderSystem] On Remove RenderCommandDomain, \"{}\" not exist", name);
		return;
	}

	m_command_domain.erase(name);
}
