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
		
		
		
		
		std::unique_ptr<Gpu::RenderPass> render{ new Gpu::RenderPass(ctx->get_device())};


		render->add_color_attachement(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.compile();

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