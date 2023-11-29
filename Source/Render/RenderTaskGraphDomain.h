#pragma once

#include "RenderTaskGraph.h"

namespace Warp::Render {

	class RenderSystem;
	class RenderTaskGraph;

	class RenderTaskGraphDomain {

		friend class RenderSystem;

		RenderTaskGraphDomain(const MString& name, RenderSystem* render_system, VkExtent2D static_resolution);

	public:
		RenderTaskGraphDomain() = delete;

		~RenderTaskGraphDomain() = default;

		RenderTaskGraphDomain(const RenderTaskGraphDomain&) = delete;

		RenderTaskGraphDomain(RenderTaskGraphDomain&&) = delete;

		RenderTaskGraphDomain& operator=(const RenderTaskGraphDomain&) = delete;

		RenderTaskGraphDomain& operator=(RenderTaskGraphDomain&&) = delete;

		RenderTaskGraphDomain& add_image_node(const MString& name, const MString& type);

		RenderTaskGraphDomain& add_buffer_node(const MString& name, const MString& type, uint64_t size);

		RenderTaskGraphDomain& add_task_node(const MString& name, const MString& type, const MVector<uint32_t>& ref_images, const MVector<uint32_t>& ref_buffers = {});

		RenderTaskGraphDomain& set_screen_resolution(const VkExtent2D& resolution);

		void clear() const;

		bool compile() const;

		inline VkExtent2D get_static_resolution() const {
			return m_resource_static_resolution;
		}

		inline RenderSystem* get_render_system() const {
			return m_render_system;
		}

		GPU::GPUImage* get_gpu_image(const MString& name, uint32_t frame_idx) const;

	private:
		MString m_name{};
		VkExtent2D m_resource_static_resolution{};
		MVector<GPU::GPUCommandBuffer*> m_command_buffers{};
		MVector<std::unique_ptr<RenderTaskGraph>> m_render_task_graphs{};
		RenderSystem* m_render_system{ nullptr };
	};
}