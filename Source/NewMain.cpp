#include "iostream"

#include "Core/Object.hpp"
#include "Core/observer_ptr.hpp"
#include "Core/Inherit.hpp"
#include "Core/Allocator.hpp"
#include "Core/TypeName.hpp"
#include "Vk/Context.hpp"
#include "Vk/GlslCompiler.hpp"
#include "Core/Math.hpp"
#include "Data/Data.hpp"
#include "llvm/ADT/BitVector.h"
#include "llvm/ir/LLVMContext.h"

using namespace Warp;
using namespace Warp::Data;

struct alignas(64) CC {
	uint8_t a;
};

struct AA {
	CC cc;
	uint8_t a;
};

extern "C" {

	struct kk {
		int8_t a;
	};

	struct BB {

		int32_t a;
		alignas(64) kk k[2];
		int32_t a1;
		int32_t a2;
	};

	struct DD {

		int32_t a;
		int8_t c2[4];
		int64_t b;
		int8_t c[9];
		BB bb;
		uint32_t cc;
		int8_t d;
		glm::mat4x4 mat;
	};

}

int main() {

	mi_option_set(mi_option_show_stats, 1);
	mi_option_set(mi_option_show_errors, 1);

	{
		std::println(std::cout, "{}", sizeof(kk));
		std::println(std::cout, "{}", sizeof(BB));
		std::println(std::cout, "{}", sizeof(DD));

		//std::unique_ptr<DataPacketDataStructureElementDescriptor> desc1{ new DataPacketDataStructureElementDescriptor (DataPacketMemberType::I8)};
		//std::unique_ptr < DataPacketDataStructureElementDescriptor> desc2{ new DataPacketDataStructureElementDescriptor(DataPacketMemberType::I32) };
		//std::unique_ptr < DataPacketDataStructureElementDescriptor> desc3{ new DataPacketDataStructureElementDescriptor(DataPacketMemberType::I32) };

		const auto desc1 = std::make_unique<DataPacketDataStructureDescriptor>("kk", "", 64);
		desc1->add_normal_member(DataPacketMemberType::I8, "a").compile();

		const auto desc2 = std::make_unique<DataPacketDataStructureDescriptor>("BB");
		desc2->add_normal_member(DataPacketMemberType::I32, "a")
			.add_structure_member(desc1.get(), "k", "", {2})
			.add_normal_member(DataPacketMemberType::I32, "a1")
			.add_normal_member(DataPacketMemberType::I32, "a2")
			.compile();

		const auto desc3 = std::make_unique<DataPacketDataStructureDescriptor>("DD");
		desc3->add_normal_member(DataPacketMemberType::I32, "a")
			.add_normal_member(DataPacketMemberType::I8, "c2", "", { 4 })
			.add_normal_member(DataPacketMemberType::I64, "b")
			.add_normal_member(DataPacketMemberType::I8, "c", "", { 9 })
			.add_structure_member(desc2.get(), "bb")
			.add_normal_member(DataPacketMemberType::I8, "d")
			.add_normal_member(DataPacketMemberType::Mat4x4, "mat")
			.compile();

		info("desc 1 is {}.", desc1->get_struct_size());
		info("desc 2 is {}.", desc2->get_struct_size());
		info("desc 3 is {}.", desc3->get_struct_size());

		desc3->recalcuate_and_recompile();
		info("desc 1 is {}.", desc1->get_struct_size());
		info("desc 2 is {}.", desc2->get_struct_size());
		info("desc 3 is {}.", desc3->get_struct_size());

		info("ssss is {}.", sizeof(DataPacketDataStructureDescriptor));

		DataBufferVector tk;

		int ad[3] = { 5, 12, 165 };
		tk.push_back(ad[0]);
		tk.push_back(ad[1]);
		tk.push_back(ad[2]);

		std::array<int, 3>& ppp = *(static_cast<std::array<int, 3>*>(tk.get_data()));

		for(auto i : ppp)
		{
			info(" array : {}.", i);
		}

		info("addresss {}.", tk.get_data());

		for (size_t i = 0; i < 3; i++) {
			uint32_t k = 0;
			if(tk.fetch_back(k))
			{
				info("d :{};", k);

			}else
			{
				info("d :{} error ;", k);

			}
		}

		DataBufferUniformVector a(4);
		a.push_back(5);
		a.push_back(12);
		a.push_back(165);

		for (uint32_t i = 0; i < 100; i++)
		{
			a.push_back(i);
		}

		info("i count :{};", a.get_count());

		for(size_t i = 0; i < a.get_count(); i++)
		{
			uint32_t* p = a.at<uint32_t>(i);
			if(p != nullptr) {
				info("{} :{};", i, *p);
			}else
			{
				info("{} : error ;", i);
			}
		}

		DataPacketDataStructreInfoPack descs{};
		bool success = descs.parse_and_replace_all(
		"struct kk 'hello, world' {		\n"
		"	i8 kk_a;				\n"
		"}					\n"
		"struct BB {				\n"
		"	i32 bb_a;				\n"
		"	kk[2] bb_b 64;			\n"
		"	i32 bb_c;				\n"
		"	i32 bb_d;				\n"
		"}					\n"
		"struct DD {				\n"
		"	i32 dd_a;				\n"
		"	i8[4] dd_b;				\n"
		"	i64 dd_c 'poposa';			\n"
		"	i8[9] dd_d;				\n"
		"	BB dd_e;				\n"
		"	i32 dd_f;				\n"
		"	i8 dd_g;				\n"
		"	mat4x4 dd_h;				\n"
		"}");

		if(success) {
			auto desc = descs.find_struct("kk");
			auto desc1 = descs.find_struct("BB");
			auto desc2 = descs.find_struct("DD");
			if(desc && desc1 && desc2)
			{
				info("kk's size is {}", desc->get_struct_size());
				info("BB's size is {}", desc1->get_struct_size());
				info("DD's size is {}", desc2->get_struct_size());
			}else
			{
				error("cant not find.");
			}

			desc2->for_each_read([](const DataPacketDataStructureElementDescriptor& item, uint8_t depth) {
				MString output = "";
				for(uint8_t i = 0; i < depth; i++) {
					output += "      ";
				}
				output += MString::format("name: {}, size: {}, offset: {}.", item._name.c_str(), item._element_size, item._element_offset);
				info(output);
			});

			{
				DataPacketLayout layout(desc2);
				bool res = layout.compile();

				auto count = layout.get_data_main_struct_memeber_count();
				for(int i = 0; i < count; i++)
				{
					auto struct_idx = layout.get_data_memeber_info_member_struct_start_idx(i);
					if(struct_idx != 0) {
						info("struct idx: {}.", struct_idx);
					}

					info("dd la name: {}, size: {}, desc: {}. strcut idx {}.", layout.get_data_member_info_name(i), layout.get_data_member_info_size(i), layout.get_data_member_info_desc_text(i), struct_idx);
				}
			}

			info("end");

		}else {
			error("parse failed.");
		}

		std::unique_ptr<Gpu::Context> ctx{ new Warp::Gpu::Context };
		auto ptr = ctx->create_window("hello", 800, 600, SDL_WINDOW_RESIZABLE);

		MVector<uint8_t> file_data;
		MVector<uint32_t> spirv{};

		size_t file_size = 0;
		void* buffer = SDL_LoadFile("../../../Shader/shader_ubo.vert", &file_size);
		info("length + {}", file_size);
		if (buffer != nullptr) {
			file_data.resize(file_size + 1);
			memcpy(file_data.data(), buffer, file_size);
			file_data[file_data.size() - 1] = 0;
			auto res = Warp::Gpu::GlslCompiler::compile_glsl_to_spirv(GLSLANG_STAGE_VERTEX, reinterpret_cast<const char*>(file_data.data()), spirv);
			if(!res.empty())
			{
				error("{}", res.c_str());
			}
			SDL_free(buffer);
		}

		auto render = std::make_unique<Gpu::RenderPass>(ctx->get_device());

		bool res = render->add_color_attachement(VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.compile();

		auto desc_layout = ctx->create_descriptor_set_layout(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
		auto desc_layout2 = ctx->create_descriptor_set_layout(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
		auto desc_layout3 = ctx->create_descriptor_set_layout(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
		auto desc_layout4 = ctx->create_descriptor_set_layout(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

		desc_layout->add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1).compile();
		desc_layout2->add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1).compile();
		desc_layout3->add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1).compile();
		desc_layout4->add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1).compile();

		std::unique_ptr<Gpu::PipelineLayout> pipeline_layout{ new Gpu::PipelineLayout(ctx->get_device()) };

		pipeline_layout->add_set_layout(desc_layout.get())
			.add_set_layout(desc_layout2.get())
			.add_set_layout(desc_layout3.get())
			.add_set_layout(desc_layout4.get())
			.compile();

		auto pool = std::make_unique<Gpu::DescriptorPool>(ctx->get_device(), 128);

		pool->add_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 128)
			.compile();

		pool->allocate_set(desc_layout.get());

		auto img = ctx->create_image_color({ 800, 600, 1 }, 0, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		auto sha = ctx->create_shader_module(spirv);

		Gpu::GraphicsPipelineState st{};
		st.set_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *sha.get())
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
			if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
				break;
			}
			if (e.type == SDL_EVENT_WINDOW_RESIZED) {
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