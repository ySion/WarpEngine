#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "Core/Core.hpp"
#include "Core/Math.hpp"

#include "Vk/Context.hpp"
#include "Vk/GlslCompiler.hpp"

#include "Data/Data.hpp"
#include "NodeSystem/NodeSystem.hpp"
#include "Engine/EngineContext.hpp"

using namespace Warp;
using namespace Warp::Data;

extern "C" {
	struct C {
		int8_t a;
	};

	struct B {

		int32_t a;
		alignas(64) C b[2];
		int32_t c;
		int32_t d;
	};

	struct A {
		int32_t a;
		int8_t b[4];
		int64_t c;
		int8_t d;
		B e;
		int32_t f;
		int8_t g;
		char str[64];
		//const char* h;
		glm::mat4x4 i;
	};
}

//
//
//using ComponentID = uint64_t;
//using ComponentList = MVector<ComponentID>;

/// 可以认为ComponentData是个表 所以说是ComponentDataTable, 列是一类component的类型, 行是多个component的并排,就像这样:
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |
/// | A | B | C | D |

//using ComponentDataTable = uint32_t; // 这个本质上是一个buffer, 这里先用这个替代
//
//using EntityID = uint64_t;
//using EntityList = MVector<EntityID>;
//
//struct Archetype;
//
//struct ArchetypeEdge {
//	Archetype* add;
//	Archetype* remove;
//};
//
//using ArchetypeID = uint64_t;
//using ArchetypeIDToArchetype = MMap<ArchetypeID, Archetype*>;
//struct Archetype {
//	ArchetypeID id;
//	ComponentList components;
//	ComponentDataTable data;
//	MMap<ComponentID, ArchetypeEdge> edges;
//};
//
//struct ComponentColumn {
//	uint32_t column;
//};
//
//using EntityIDToArchetypeMap = MMap<EntityID, Archetype*>;
//
//using ComponentColumnInArchetype = MMap<ArchetypeID, ComponentColumn>;
//using ComponentIDToArchetypeIDMap = MMap<ComponentID, ComponentColumnInArchetype>;
//
//struct ecs_world {
//	EntityList _entities;
//	ArchetypeIDToArchetype _archetype_id_to_archetype_map;
//	EntityIDToArchetypeMap _entity_to_archetype_map;
//	ComponentIDToArchetypeIDMap _component_to_archetype_map;
//};

class GClass {
public:
	GClass() {
		info("hello");
	}

	~GClass() {
		info("bye");
	}

	GClass(const GClass&) {
		info("copy");
	}

	GClass(GClass&&) {
		info("move");
	}
};

int main() {
	mi_version();

	mi_option_set(mi_option_verbose, 1);
	mi_option_set(mi_option_show_stats, 1);
	mi_option_set(mi_option_show_errors, 1);
	mi_option_set(mi_option_show_errors, 1);
	mi_option_set(mi_option_reserve_huge_os_pages, 4);

	NodeSystem::NodeDescriptor node("hello");

	auto& graph = *node.get_context();

	graph.add_node("Box").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_output_pin(NodeSystem::PinType::Thin, "o1");
		return std::optional{ node };
	});

	graph.add_node("In1").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_output_pin(NodeSystem::PinType::Single, "o1");
		return std::optional{ node };
	});

	graph.add_node("In2").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_output_pin(NodeSystem::PinType::Single, "o1");
		return std::optional{ node };
	});

	graph.add_node("Trans").and_then([](NodeSystem::NodeDescriptor* node)
	{
		node->add_input_pin(NodeSystem::PinType::Thin, "i1");
		node->add_output_pin(NodeSystem::PinType::Thin, "o1");
		node->add_output_pin(NodeSystem::PinType::Thin, "o2");
		return std::optional{node};
	});

	graph.add_node("Core").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_input_pin(NodeSystem::PinType::Single, "i1");
		node->add_input_pin(NodeSystem::PinType::Width, "i2");
		node->add_output_pin(NodeSystem::PinType::Thin, "o1");
		node->add_output_pin(NodeSystem::PinType::Thin, "o2");
		return std::optional{ node };
	});

	graph.add_node("Out1").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_input_pin(NodeSystem::PinType::Thin, "i1");
		node->add_output_pin(NodeSystem::PinType::Thin, "o1");
		return std::optional{ node };
	});

	graph.add_node("Out2").and_then([](NodeSystem::NodeDescriptor* node) {
		node->add_input_pin(NodeSystem::PinType::Thin, "i1");
		node->add_output_pin(NodeSystem::PinType::Thin, "o1");
		return std::optional{ node };
	});


	graph.add_edge("Box", "o1", "Trans", "i1");
	graph.add_edge("Box", "o1", "Core", "i1");

	graph.add_edge("In1", "o1", "Core", "i2");
	graph.add_edge("In2", "o1", "Core", "i2");

	graph.add_edge("Trans", "o2", "Out1", "i1");
	graph.add_edge("Core", "o1", "Out1", "i1");

	graph.add_edge("Core", "o1", "Out2", "i1");

	//graph.add_edge("Out1", "o1", "Trans", "i1");


	if (graph.is_graph_has_depth_cycling()) {
		printf("has depth cycle\n");
	} else {
		printf("no depth cycle\n");

	}

	if (graph.is_graph_has_cycling(NodeSystem::ContextDescriptor::Sequence::Value)) {
		printf("has cycle\n");
	} else {
		graph.visit("Out1", NodeSystem::ContextDescriptor::Sequence::Value);
	}

	info("remove graph");

	graph.remove_node("Box");

	if (graph.is_graph_has_cycling(NodeSystem::ContextDescriptor::Sequence::Value)) {
		printf("has cycle\n");
	} else {
		graph.visit("Out1", NodeSystem::ContextDescriptor::Sequence::Value);
	}

	auto new_graph = graph.clone();

	info("new graph");

	if (new_graph->is_graph_has_cycling(NodeSystem::ContextDescriptor::Sequence::Value)) {
		printf("has cycle\n");
	} else {
		graph.visit("Out1", NodeSystem::ContextDescriptor::Sequence::Value);
	}

	info("done");
	
	//nodes.add_node("A", "", 2, 2);
	//nodes.add_node("B", "", 2, 2);
	//nodes.add_node("C", "", 2, 2);
	//nodes.add_node("D", "", 2, 2);
	//nodes.add_node("E", "", 2, 2);


	//nodes.add_edge("A", "B", 0, 0);
	//nodes.add_edge("A", "C", 1, 0);
	//nodes.add_edge("B", "C", 0, 1);
	//nodes.add_edge("C", "D", 0, 0);
	//nodes.add_edge("C", "E", 1, 0);

	//nodes.debug_calc_reverse("E");

	{

		DataLayoutStructureDescriptorPack desc_info{};
		bool success = desc_info.parse_and_replace_all(
		""
		"struct C 'hello, world'{		\n"
		"	i8 a;				\n"
		"}					\n"
		"struct B {				\n"
		"	i32 a;				\n"
		"	C[2] b 64;			\n"
		"	i32 c;				\n"
		"	i32 d;				\n"
		"}					\n"
		"struct A {				\n"
		"	i32 a;				\n"
		"	i8[4] b;			\n"
		"	i64 c 'this is a desc';		\n"
		"	i8 d;				\n"
		"	B e;				\n"
		"	i32 f;				\n"
		"	i8 g;				\n"
		"	char[64] str;			\n"
		"	mat4x4 i;			\n"
		"}");
		if (!success) return 0;

		const auto engine_ctx = std::make_unique<Engine::EngineContext>();
		auto layout = engine_ctx->get_data_layout_manager()->create_layout(desc_info.find_struct("A"), "hello");
		auto des_manager = engine_ctx->get_densify_system_manager();


		DataLayoutStructureDescriptorPack name_struct{};
		bool parse_success = name_struct.parse_and_replace_all(
		"struct PersonComponent {		\n"
		"	char[32] name;				\n"
		"	i32 health;				\n"
		"	i32 magic_level;			\n"
		"}"
		);

		if (!parse_success) return 0;
		auto person_struct_layout = engine_ctx->get_data_layout_manager()->create_layout(name_struct.find_struct("PersonComponent"), "PersonComponent");
		des_manager->create_component("People", person_struct_layout);
		des_manager->create_component("People2", person_struct_layout);
		des_manager->create_component("People3", person_struct_layout);
		des_manager->create_component("People4", person_struct_layout);
		des_manager->create_component("People5", person_struct_layout);
		des_manager->create_component("People6", person_struct_layout);
		des_manager->create_component("People7", person_struct_layout);
		des_manager->create_component("People8", person_struct_layout);
		des_manager->create_component("People9", person_struct_layout);
		des_manager->create_component("People10", person_struct_layout);

		const auto first = des_manager->create_tag_component("LiveIn");
		des_manager->create_tag_component("Town");
		des_manager->create_tag_component("City");
		des_manager->create_tag_component("Country");
		des_manager->create_tag_component("Friendof");
		des_manager->create_tag_component("Hero");
		des_manager->create_tag_component("Name");
		des_manager->create_pair_component("LiveIn", "Town");
		des_manager->create_pair_component("LiveIn", "City");
		des_manager->create_pair_component("LiveIn", "Country");

		const auto setp = des_manager->get_pair_component_A_to_B_set(first.value());

		for(const auto i : *setp) {
			const auto layout_tempB = des_manager->get_component_layout(i);
			info("{}", layout_tempB->get_name());
		}

		auto collection1 = des_manager->create_collection("Test1");

		auto bb = collection1->command_create_single_entity("Own3");
		auto bb2 = collection1->command_create_single_entity("Own4");

		auto alice = collection1->command_create_single_entity("Alice");
		auto elissia = collection1->command_create_single_entity("Elissia");
		auto like = collection1->command_create_single_entity("Like");
		collection1->command_create_single_entity("Apple");
		collection1->command_create_single_entity("Boat");
		collection1->command_create_single_entity("Roya");
		collection1->command_create_single_entity("Own");
		collection1->create_pair_component_dynamic("Hero", "Elissia");
		collection1->create_pair_component_dynamic("Hero", "Alice");
		collection1->create_pair_component_dynamic("Like", "Alice");
		collection1->create_pair_component_dynamic("Like", "Apple");
		auto sid = collection1->create_pair_component_dynamic("Like", "Elissia");
		collection1->create_pair_component_dynamic("Own", "Boat");

		collection1->command_create_single_entity("Own2");

		collection1->command_remove_signle_entity(bb);
		collection1->command_remove_signle_entity(bb2);
		/*collection1->try_register_component_from_static("Own");
		collection1->try_register_component_from_static("People");
		collection1->try_register_component_from_static("People2");
		collection1->try_register_component_from_static("People3");
		collection1->try_register_component_from_static("People4");
		collection1->try_register_component_from_static("People5");
		collection1->try_register_component_from_static("People6");
		collection1->try_register_component_from_static("People7");
		collection1->try_register_component_from_static("People8");
		collection1->try_register_component_from_static("People9");
		collection1->try_register_component_from_static("People10");*/

		WARP_BENCH(T1);
		auto entities = collection1->commnad_create_entities(1000000, 5);
		WARP_BENCH(T2);
		collection1->command_record_add_entities_components_by_component_name(entities, { "Own", "People", "People2" });
		//collection1->command_record_add_entities_components_by_component_name({ alice }, { "People", "Name", "(Like,Apple)", "(Like,Elissia)" });
		//collection1->command_record_add_entities_components_by_component_name({ elissia }, { "People", "Name", "(Own,Boat)" });
		//collection1->command_record_remove_entities_components_by_component_name(entities, { "Own" });
		WARP_BENCH(T3);
		collection1->submit_command();
		WARP_BENCH(T4);
		collection1->command_record_add_entities_components_by_component_name(entities, { "People3", "People4" });
		WARP_BENCH(T5);
		collection1->submit_command();
		WARP_BENCH(T6);
		collection1->command_record_add_entities_components_by_component_name(entities, { "People5", "People6" });
		WARP_BENCH(T7);
		collection1->submit_command();
		WARP_BENCH(T8);
		collection1->command_record_add_entities_components_by_component_name(entities, { "People7", "People8" });
		WARP_BENCH(T9);
		collection1->submit_command();
		WARP_BENCH(T10);
		collection1->command_record_add_entities_components_by_component_name(entities, { "People9", "People10" });
		WARP_BENCH(T11);
		collection1->submit_command();
		WARP_BENCH(T12);
		collection1->submit_command();
		WARP_BENCH(T13);
		collection1->submit_command();
		WARP_BENCH(T14);
		collection1->submit_command();
		WARP_BENCH(T15);

		WARP_BENCH_INFO("Create Entities", T1, T2);

		WARP_BENCH_INFO("第1次 Make Commands", T2, T3);
		WARP_BENCH_INFO("第1次 Command Submit", T3, T4);
		WARP_BENCH_INFO("第1次Total", T2, T4);

		WARP_BENCH_INFO("第2次 Maker Command", T4, T5);
		WARP_BENCH_INFO("第2次 Command Submit", T5, T6);
		WARP_BENCH_INFO("第2次 Total", T4, T6);

		WARP_BENCH_INFO("第3次 Maker Command", T6, T7);
		WARP_BENCH_INFO("第3次 Command Submit", T7, T8);
		WARP_BENCH_INFO("第3次 Total", T6, T8);

		WARP_BENCH_INFO("第4次 Maker Command", T8, T9);
		WARP_BENCH_INFO("第4次 Command Submit", T9, T10);
		WARP_BENCH_INFO("第4次 Total", T8, T10);

		WARP_BENCH_INFO("第5次 Maker Command", T10, T11);
		WARP_BENCH_INFO("第5次 Command Submit", T11, T12);
		WARP_BENCH_INFO("第5次 Total", T10, T12);


		WARP_BENCH_INFO("第6次 submit 空", T12, T13);
		WARP_BENCH_INFO("第7次 submit 空", T13, T14);
		WARP_BENCH_INFO("第8次 submit 空", T14, T15);
		

		for (const auto i : collection1->get_view_archetypes_id()) {
			info("Archetype: {}, {}", i, collection1->get_archetype_struct_desc(i).value());
		}

		return 0;

		info("Alice Have Component ? {}", collection1->is_entity_has_component(alice, sid.value()));
		
		info("See:");
		for(const auto& i : collection1->get_view_components_name()) {
			info("{}", i);
		}
		info("=====================");
		//return 0;

		info("{}", layout->get_all_element_count());
		info("{}", layout->get_main_struct_memeber_count());

		info("have {} text, {} ptr", layout->get_memeber_document_count(), layout->get_memeber_ptr_count());

		info("data size {} as {}, struct info desc segment size {}", layout->get_data_segment_size(), sizeof(A), layout->get_info_segment_size());

		info("alignd {} ", offsetof(A, i));

		layout->visit([&layout](uint16_t idx, uint16_t depth) {

			MString tab = "";
			for (uint16_t i = 0; i < depth; i++) {
				tab += "\t";
			}

			info("{}name {} type {} size {} alignment {} array dim {}, =& {}.", tab,
			layout->get_member_name(idx),
			data_packet_basic_type_type_to_string(layout->get_memeber_type(idx)),
			layout->get_member_size(idx).value(),
			layout->get_memeber_alignment(idx).value(),
			layout->get_memeber_array_dimension(idx).value_or(0),
			layout->get_member_data_start_address_offset(idx).value()
			);
		});

		for (auto& i : layout->get_name_mapped_to_idx_offset()) {
			info("{} idx {}, add offset {}", i.first, i.second.idx_offset, i.second.address_offset);
		}

		auto packet = Data::DataLayoutUnorderedBlockVector::make(layout);
		packet->push_back(1);
		//packet->set_data_document("h", 0, "hello world");
		
		info("mem size {}.", packet->size_used_memory());
		info("seg size {}.", packet->get_layout()->get_data_segment_size());
		auto some_struct_in_memory = packet->at<A>(0);
		info("read: {} raw: {}", packet->get_data_start_address("e.a", 0), (void*)some_struct_in_memory);

		*packet->get_data_start_address<int32_t>("e.a", 0) = 66;
		strcpy(packet->get_data_start_address<char>("str", 0), "What you want to get");

		//some_struct_in_memory->e.a = 25;

		if (some_struct_in_memory != nullptr) {
			//info("raw e.a address {}, value {}.", (void*)&(some_struct_in_memory->e.a), some_struct_in_memory->e.a);
			//info("read e.a address {}, value {}.", (void*)packet->get_data_start_address<int32_t>("e.a", 0) ,*packet->get_data_start_address<int32_t>("e.a", 0));
			info("e.a is {}.", some_struct_in_memory->e.a);
			info("h is {}.", some_struct_in_memory->str);
		}


		std::unique_ptr<Gpu::Context> ctx{ new Warp::Gpu::Context };
		auto ptr = ctx->create_window("hello", 800, 600, SDL_WINDOW_RESIZABLE);

		MVector<uint8_t> file_data;
		MVector<uint32_t> spirv{};

		size_t file_size = 0;
		void* buffer = SDL_LoadFile("../../../Shader/shader_ubo.vert", &file_size);
		if (buffer != nullptr) {
			file_data.resize(file_size + 1);
			memcpy(file_data.data(), buffer, file_size);
			file_data[file_data.size() - 1] = 0;
			auto res = Warp::Gpu::GlslCompiler::compile_glsl_to_spirv(GLSLANG_STAGE_VERTEX, reinterpret_cast<const char*>(file_data.data()), spirv);
			if (!res.empty()) {
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

		auto sha = ctx->create_shader_module(Gpu::ShaderType::Vertex, spirv);

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
			std::cout << std::format("Object Manager: type \"{}\" at address {}, \t have {} objects, \tsize:{} Bytes \n", name, (void*)ptr, ptr->get_object_count(), ptr->get_object_memory_use());
		}
		printf("=====================\n");
	}

	printf("=====================\n");
	auto b = AllocatorGroup::get_instance()->get_all_allocators();
	for (const auto& [name, ptr] : b) {
		std::cout << std::format("Object Manager: type \"{}\" at address {},  \t have {} objects\n", name, (void*)ptr, ptr->get_object_count());
	}
	printf("=====================\n");
	return 0;
		}