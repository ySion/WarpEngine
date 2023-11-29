#pragma once

#include "RenderResourceGroup.h"
#include "RenderTaskGraphDomain.h"
#include "RenderCommandDomain.h"

namespace Warp::Render {

	class RenderSystem {
	public:
		RenderSystem();

		~RenderSystem() = default;

		RenderSystem(const RenderSystem&) = delete;

		RenderSystem& operator=(const RenderSystem&) = delete;

		RenderSystem(RenderSystem&&) = delete;

		RenderSystem& operator=(RenderSystem&&) = delete;

		RenderTaskGraphDomain* make_render_task_graph_domain(const MString& name, VkExtent2D resolution);

		void remove_render_task_graph_domain(const MString& name);

		RenderCommandDomain* make_command_domain(const MString& name);

		void remove_command_domain(const MString& name);

		inline RenderResourceGroup* get_render_resource_group() const { return m_render_resource_group.get(); }


	private:
		std::unique_ptr<RenderResourceGroup> m_render_resource_group{};

		MMap<MString, std::unique_ptr<RenderTaskGraphDomain>> m_render_task_graph_domains{};

		MMap<MString, std::unique_ptr<RenderCommandDomain>> m_command_domain{};

		friend class RenderTaskGraph;
	};

}
