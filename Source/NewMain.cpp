#include "iostream"

#include "Core/Object.hpp"
#include "Core/observer_ptr.hpp"
#include "Core/Inherit.hpp"
#include "Core/Allocator.hpp"
#include "Core/TypeName.hpp"
#include "Core/MIStl.h"
#include "Vk/Context.hpp"
#include "Vk/GlslCompiler.hpp"
#include "Vk/RenderPass.hpp"
#include "Vk/DescriptorSetLayout.hpp"
#include "Vk/DescriptorPool.hpp"
#include "Vk/PipelineLayout.hpp"
#include "Vk/Image.hpp"


using namespace Warp;


int main() {

	{
		std::unique_ptr<Gpu::Context> ctx{ new Warp::Gpu::Context };
		auto ptr = ctx->create_window("hello", 800, 600, SDL_WINDOW_RESIZABLE);

		std::vector<uint8_t> file_data;
		size_t file_size = 0;
		void* buffer = SDL_LoadFile("../../../Shader/shader.vert", &file_size);

		if(buffer != nullptr) {
			file_data.resize(file_size);
			memcpy(file_data.data(), buffer, file_size);
			std::vector<uint8_t> spirv{};
			Warp::Gpu::GlslCompiler::compile_glsl_to_spirv(GLSLANG_STAGE_VERTEX, reinterpret_cast<const char*>(file_data.data()), spirv);
			SDL_free(buffer);
		}

		auto render= std::make_unique<Gpu::RenderPass>(ctx->get_device());

		bool res = render->add_color_attachement(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.compile();

		auto desc_layout = ctx->create_descriptor_set_layout(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

		desc_layout->add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1)
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
			.compile();

		std::unique_ptr<Gpu::PipelineLayout> pipeline_layout{ new Gpu::PipelineLayout(ctx->get_device()) };

		pipeline_layout->add_set_layout(desc_layout.get())
			.compile();

		auto pool = std::make_unique<Gpu::DescriptorPool>(ctx->get_device(), 128);

		pool->add_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 128)
			.compile();

		pool->allocate_set(desc_layout.get());

		auto img = ctx->create_image_color({ 800, 600, 1 }, 0, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		Gpu::GraphicsPipelineState st{};
		st.set_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.set_tessellation()
			.set_multisample(VK_SAMPLE_COUNT_1_BIT)
			.set_rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0)
			.set_depth_stencil(false, false, VK_COMPARE_OP_LESS)
			.add_dynamic_state(VK_DYNAMIC_STATE_VIEWPORT)
			.add_dynamic_state(VK_DYNAMIC_STATE_SCISSOR)
			.add_color_blend_attachment();

		auto grap = ctx->create_graphics_pipeline(&st, render.get(), pipeline_layout.get());

		SDL_Event e;
		while (true) {
			SDL_PollEvent(&e);
			if(e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
				break;
			}
			if(e.type == SDL_EVENT_WINDOW_RESIZED) {
				ptr->set_extent(e.window.data1, e.window.data2);
				printf("resize to %d %d\n", e.window.data1, e.window.data2);
			}
		}
		
		printf("=====================\n");
		auto b = AllocatorGroup::get_instance()->get_all_allocators();
		for (const auto& [name, ptr] : b) {
			std::cout << std::format("Object Manager: type \"{}\" at address {}, have {} objects\n", name, (void*)ptr, ptr->get_object_count());
		}
		printf("=====================\n");
	}

	printf("=====================\n");
	auto b = AllocatorGroup::get_instance()->get_all_allocators();
	for (const auto& [name, ptr] : b) {
		std::cout << std::format("Object Manager: type \"{}\" at address {}, have {} objects\n", name, (void*)ptr, ptr->get_object_count());
	}
	printf("=====================\n");
	return 0;
}