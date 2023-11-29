#pragma once

#include "Graphics/GPUResource.h"

namespace Warp::Render {

	class RenderSystem;

	class RenderCommandDomain {
		friend class RenderSystem;

		RenderCommandDomain(const MString& name, RenderSystem* render_system);
	public:
		RenderCommandDomain(const RenderCommandDomain&) = delete;

		RenderCommandDomain(RenderCommandDomain&&) = delete;

		RenderCommandDomain& operator=(const RenderCommandDomain&) = delete;

		RenderCommandDomain& operator=(RenderCommandDomain&&) = delete;

		~RenderCommandDomain() = default;

	public:

		inline GPU::GPUCommandBuffer* acquire_command_buffer() const {
			return m_cmd_buffers[m_current_frame_index].get();
		}

		//inline void execute_command_buffer() const {
		//	//m_cmd_buffers[current_frame_index]->submit;
		//}

	private:

		MString m_name{};

		uint32_t m_current_frame_index{ 0 };

		RenderSystem* m_render_system{ nullptr };

		GPU::GPUResourceHandle<GPU::GPUCommandPool> m_command_pool{ nullptr };

		MVector<GPU::GPUResourceHandle<GPU::GPUCommandBuffer>> m_cmd_buffers{ 3 };
	};
}