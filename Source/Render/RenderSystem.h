#pragma once

#include "RenderResourceGroup.h"
#include "RenderTaskGraphDomain.h"

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

		inline RenderResourceGroup* get_render_resource_group() const { return m_render_resource_group.get(); }

		inline GPU::GPUCommandBuffer* acquire_command_buffer() const {
			return m_cmd_pool->m_command_buffers[m_current_frame_index].get();
		}

		inline GPU::GPUCommandPool* get_command_pool() const {
			return m_cmd_pool.get();
		}

	private:
		std::unique_ptr<RenderResourceGroup> m_render_resource_group{};

		MMap<MString, std::unique_ptr<RenderTaskGraphDomain>> m_render_task_graph_domains{};

		uint32_t m_current_frame_index{ 0 };

		GPU::GPUResourceHandle<GPU::GPUCommandPool> m_cmd_pool{};

		friend class RenderTaskGraph;
	};

}
