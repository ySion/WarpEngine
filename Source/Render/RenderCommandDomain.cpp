#include "RenderCommandDomain.h"
#include "RenderSystem.h"

Warp::Render::RenderCommandDomain::RenderCommandDomain(const MString& name, RenderSystem* render_system) : m_name(name), m_render_system(render_system) {

	const auto res_mag = m_render_system->get_render_resource_group();

	MString pool_name = mformat("{}_command_pool", name);
	const auto ptr = res_mag->get_command_pool_manager()->builder(pool_name).set_configs(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT).make(true);

	m_command_pool.reset(ptr);


	for (int i = 0; i < 3; i++) {
		MString buffer_name = mformat("{}_command_buffer_{}", name, i);
		m_cmd_buffers[i] = res_mag->get_command_buffer_manager()->builder(buffer_name)
			.set_configs(ptr).make();
	}
}
