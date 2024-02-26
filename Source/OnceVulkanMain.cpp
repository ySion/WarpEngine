#include "Graphics/VulkanFiles.h"

#include <iostream>
#include <vector>
#include <ranges>

#include "Core/Log.h"
#include "Core/Utils.h"

#include "Core/Utils.h"
#include "Core/Math.hpp"
#include "Loaders/tiny_obj_loader.h"


#include "Render/RenderSystem.h"

#include "Image/image.h"

#include "lmdb.h"

int main3() {

	Warp::GPU::GPUFactoryCreateInfo create_info{
		{
			"VK_KHR_surface",
			"VK_KHR_win32_surface"
		},
		{
			"VK_LAYER_KHRONOS_validation"
		},
		{
			"VK_KHR_swapchain",
			"VK_KHR_get_memory_requirements2",
			"VK_KHR_dedicated_allocation",
			"VK_KHR_bind_memory2"
		}
	};

	Warp::GPU::GPUFactory::init(create_info);
	{
		Warp::Render::RenderSystem rendersystem{};

		const auto render_graph = rendersystem.make_render_task_graph_domain("test", { 1920,1080 });

		const auto success = render_graph->add_image_node("image_1", "format_r8g8b8a8_srgb")
			.add_task_node("one_make_triangle", "TaskNode_1", { 0 })
			.compile();
	}

	Warp::GPU::GPUFactory::exit();
}


class Base {
public:
	virtual void func() = 0;

	virtual ~Base() = default;
};


class Son : public Base {
public:
	Son() = default;

	void func() override {
		println("What?");
	}
};


int main1() {

	Base* hello = new Son();
	hello->func();



	MDB_env* db{};
	mdb_env_create(&db);
	mdb_env_set_maxdbs(db, 64);
	if (mdb_env_open(db, ".", 0, 0)) {
		__debugbreak();
	}

	MDB_txn* txn{};
	if (mdb_txn_begin(db, nullptr, 0, &txn)) {
		__debugbreak();
	}

	MDB_dbi dbi{};
	if (auto res = mdb_dbi_open(txn, "hello", MDB_CREATE, &dbi)) {
		printf("%d", res);
		__debugbreak();
		//error
	}

	int a = 555;
	MDB_val val{
		.mv_size = sizeof(uint32_t),
		.mv_data = &a
	};


	MDB_val key{};
	MDB_val data{};


	MDB_cursor* cursor;
	mdb_cursor_open(txn, dbi, &cursor);
	while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == 0) {
		printf("%d", *(uint32_t*)data.mv_data);
	}

	//int i = mdb_get(txn, dbi, &val, &data);
	//printf("%d", *(uint32_t*)data.mv_data);

	//mdb_txn_commit(txn);
	//printf("%d", *(uint32_t*)data.mv_data);

	mdb_cursor_close(cursor);
	mdb_dbi_close(db, dbi);
	mdb_env_close(db);

	return 0;
}

int main2() {

	Warp::GPU::GPUFactoryCreateInfo create_info{
		{
			"VK_KHR_surface",
			"VK_KHR_win32_surface"
		},
		{
			"VK_LAYER_KHRONOS_validation"
		},
		{
			"VK_KHR_swapchain",
			"VK_KHR_get_memory_requirements2",
			"VK_KHR_dedicated_allocation",
			"VK_KHR_bind_memory2"
		}
	};

	Warp::GPU::GPUFactory::init(create_info);

	const auto max_swap_image = Warp::GPU::GPUFactory::get_swap_chain_image_count();
	const auto command_frame_buffer_count = Warp::GPU::GPUFactory::get_frame_command_buffer_count();

	const auto command_fence_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUFence>("fence_manager 0");

	const auto command_semaphore_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUSemaphore>("semaphore_manager 0");

	const auto command_pool_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUCommandPool>("command_pool_manager 0");

	const auto layout_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUPipelineLayout>("pipeline_layout_manager 0");

	const auto shader_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUShader>("shader_manager 0");

	const auto renderpass_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPURenderPass>("render_pass_manager 0");

	const auto pipeline_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUGraphicsPipeline>("graphics_pipeline_manager 0");

	const auto frame_buffer_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUFrameBuffer>("frame_buffer_manager 0");

	const auto swap_chain_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUSwapChain>("swapchain_manager 0");

	const auto image_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUImage>("image_manager 0");

	const auto buffer_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUBuffer>("buffer_manager 0");

	const auto descpool_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUDescriptorPool>("descpool_manager 0");

	const auto desc_layout_manager =
		Warp::GPU::GPUFactory::make_manager<Warp::GPU::GPUResourceTypes::GPUDescriptorSetLayout>("desclayout_manager 0");

	auto swap1 = swap_chain_manager->builder("hello")
		.set_configs({ 800, 600 }, "hello world", SDL_WINDOW_RESIZABLE).make();

	auto one_layout = desc_layout_manager->builder("test_layout").
	add_descriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT).make();

	auto one_desc_pool = descpool_manager->builder("test_desc_pool")
	.add_pool_size_info(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
	.add_pool_size_info(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
	.make();

	auto one_set =  one_desc_pool->make_descriptor_set(one_layout);

	const auto renderpass = renderpass_manager->builder("main_renderpass")
		.add_attachment(VK_FORMAT_B8G8R8A8_SRGB, VK_SAMPLE_COUNT_8_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.add_attachment(VK_FORMAT_B8G8R8A8_SRGB, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.add_subpass({
			.color_attachment_refs = {0},
			.input_attachment_refs = {},
			.resolve_attachment_refs = {1},
			.depth_stencil_resolve_attachment_ref = {},
			.preserve_attachment_refs = {}
		}).make();


	auto msaa_img = image_manager->builder("msaa_img")
		.template_color_2D({ swap1->m_extent.width, swap1->m_extent.height}, VK_FORMAT_B8G8R8A8_SRGB, VK_SAMPLE_COUNT_8_BIT)
		.make();

	auto semaphore_wait_for = command_semaphore_manager->builder("semaphore_wait_for").make_multi(command_frame_buffer_count);
	auto semaphore_render_ok = command_semaphore_manager->builder("semaphore_render_ok").make_multi(command_frame_buffer_count);
	auto fences = command_fence_manager->builder("fences").make_multi(command_frame_buffer_count);

	command_semaphore_manager->find_by_name_start_with("hello");
	if (semaphore_wait_for.empty() || semaphore_render_ok.empty() || fences.empty()) {
		println("error in sync objects create");
		exit(0);
	}

	auto pool = command_pool_manager->builder("main_command_pool").set_configs(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT).make();

	MVector<Warp::GPU::GPUCommandBuffer*> cmd_buffer{};
	for(int i = 0; i < 3; ++i) {
		auto temp = pool->make_command_buffer();
		cmd_buffer.push_back(temp);
	}


	struct MeshPushConstants {
		glm::vec4 color1{ 0.1, 0.5, 1.0f, 1.0 };
		glm::vec4 color2;
		glm::vec4 color3;
		glm::vec4 color4;
	};
	auto layout = layout_manager->builder().add_push_constant({
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(MeshPushConstants)
	}).make();

	auto vert_sh = shader_manager->builder("shader_1").make_from_file("../../../Shader/shader.vert", EShLangVertex);
	auto frag_sh = shader_manager->builder("shader_2").make_from_file("../../../Shader/shader.frag", EShLangFragment);

	//auto vert_sh = shader_manager->builder("shader_1").make_from_file("shader.vert", EShLangVertex);
	//auto frag_sh = shader_manager->builder("shader_2").make_from_file("shader.frag", EShLangFragment);


	if (!vert_sh || !frag_sh) {
		println("error in shader");
	}

	auto pipeline = pipeline_manager->builder("main_pipeline")
		.set_render_pass(renderpass, 0)
		.set_state_shaders(vert_sh, nullptr, nullptr, nullptr, frag_sh)
		.set_state_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
		.set_state_multisample(VK_SAMPLE_COUNT_8_BIT)
		.set_state_rasterization(VK_POLYGON_MODE_FILL)
		.add_color_blend_attachment()
		.set_state_color_blend()
		.set_layout(layout)
		.make();

	auto pipeline_frame = pipeline_manager->builder("main_pipeline_frame")
		.set_render_pass(renderpass, 0)
		.set_state_shaders(vert_sh, nullptr, nullptr, nullptr, frag_sh)
		.set_state_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
		.set_state_multisample(VK_SAMPLE_COUNT_8_BIT)
		.set_state_rasterization(VK_POLYGON_MODE_LINE)
		.add_color_blend_attachment()
		.set_state_color_blend()
		.set_layout(layout)
		.make();

	std::vector<Warp::GPU::GPUResourceHandle<Warp::GPU::GPUFrameBuffer>> frame_buffers{};

	for (size_t i = 0; i < max_swap_image; i++) {
		auto t = frame_buffer_manager->builder()
			.set_extent(swap1->m_extent)
			.add_attachment(msaa_img)
			.add_attachment(swap1->m_image_view[i])
			.set_render_pass(renderpass).make();
		frame_buffers.emplace_back(t);
	}

	swap1->m_callback_on_resize = [&](const Warp::GPU::GPUSwapChain* swap) {

		frame_buffers.clear();

		msaa_img = image_manager->builder("msaa_img")
			.template_color_2D({ swap1->m_extent.width, swap1->m_extent.height }, VK_FORMAT_R8G8B8A8_SRGB, VK_SAMPLE_COUNT_8_BIT)
			.make(true);

		for (size_t i = 0; i < max_swap_image; i++) {

			auto t = frame_buffer_manager->builder("main_frame_buffer", i)
				.set_extent(swap1->m_extent)
				.add_attachment(msaa_img)
				.add_attachment(swap1->m_image_view[i])
				.set_render_pass(renderpass).make();
			frame_buffers.emplace_back(t);
		}
	};

	uint32_t cmd_idx = 0;
	bool is_frame = false;

	MeshPushConstants hello_data{};
	auto record_commands =
		[&](Warp::GPU::GPUCommandBuffer* cmdbuf, Warp::GPU::GPUFrameBuffer* framebuf,
		Warp::GPU::GPUSemaphore* wait_for, Warp::GPU::GPUSemaphore* render_ok, Warp::GPU::GPUFence* cmdbuf_submit_success_fence) {

			cmdbuf->begin_command_buffer();

			VkRect2D render_area{
				.offset = {0, 0},
				.extent = framebuf->m_extent
			};

			VkClearValue clsv[] = {
				{
					.color = {0.0f, 0.0f, 0.0f, 1.0f},
				},
				{
					.color = {0.0f, 0.0f, 0.0f, 1.0f},
				}
			};

			cmdbuf->begin_render_pass(renderpass, framebuf, render_area, clsv);
			if (!is_frame) {
				cmdbuf->bind_graphics_pipeline(pipeline);
			} else {
				cmdbuf->bind_graphics_pipeline(pipeline_frame);
			}

			VkViewport view_ports[] = { {
				.x = 0.0f,
				.y = 0.0f,
				.width = static_cast<float>(framebuf->m_extent.width),
				.height = static_cast<float>(framebuf->m_extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			} };
			cmdbuf->set_viewports(view_ports);

			VkRect2D scissors[] = { {
				.offset = {0,0},
				.extent = {framebuf->m_extent.width, framebuf->m_extent.height}
			} };

			cmdbuf->set_scissors(scissors);

			vkCmdPushConstants(cmdbuf->m_command_buffer, pipeline->m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &hello_data);


			cmdbuf->draw(3, 1, 0, 0);

			cmdbuf->end_render_pass();
			cmdbuf->end_command_buffer();
			cmdbuf->submit_config_seamphores_wait_for({ wait_for->m_semaphore }, { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT });
			cmdbuf->submit_config_seamphores_signal_to({ render_ok->m_semaphore });
			cmdbuf->submit(cmdbuf_submit_success_fence);
		};


	SDL_Event s_event{};
	while (true) {
		SDL_PollEvent(&s_event);
		if (s_event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			auto ptr = SDL_GetWindowFromID(s_event.window.windowID);
			if (auto res = swap_chain_manager->searcher()->find_by_window_ptr(ptr)) {
				Warp::GPU::wait_device_idle();
				frame_buffer_manager->clear();
				swap_chain_manager->erase(res.value());
				if (swap_chain_manager->is_empty()) {
					break;
				}
			}
		}if (s_event.type == SDL_EVENT_WINDOW_RESIZED) {
			auto ptr = SDL_GetWindowFromID(s_event.window.windowID);
			if (auto res = swap_chain_manager->searcher()->find_by_window_ptr(ptr)) {
				auto& obj_ptr = *res.value();
				Warp::GPU::wait_device_idle();
				swap_chain_manager->builder().make_resize_only(&obj_ptr);
				println("{} resize to {} x {}", obj_ptr.get_name().c_str(), obj_ptr.m_extent.width, obj_ptr.m_extent.height);
			}
		} else if (s_event.type == SDL_EVENT_KEY_DOWN) {
			if (s_event.key.keysym.sym == SDLK_a) {
				is_frame = true;
			} else if (s_event.key.keysym.sym == SDLK_s) {
				is_frame = false;
			}
		} else {
			Warp::GPU::wait_for_fences({ fences[cmd_idx] });
			uint32_t frame_idx = swap1->acquire_next_image(semaphore_wait_for[cmd_idx]);
			Warp::GPU::reset_fences({ fences[cmd_idx] });
			record_commands(cmd_buffer[cmd_idx], frame_buffers[frame_idx].get(), semaphore_wait_for[cmd_idx], semaphore_render_ok[cmd_idx], fences[cmd_idx]);
			swap1->present({ semaphore_render_ok[cmd_idx] });
			cmd_idx = (cmd_idx + 1) % command_frame_buffer_count;
		}
	}

	return 0;
}

int main() {

	struct objInfo {
		glm::mat4 V;
		glm::mat4 P;

	};
	uint32_t width = 1280, height = 760;
	VkSampleCountFlagBits multisample_setting = VK_SAMPLE_COUNT_8_BIT;


	objInfo OBJ_INFO{};

	OBJ_INFO.V = glm::lookAt(glm::vec3(0.0f, 3.0f, 3.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0, -1.0f, 0.0f));
	OBJ_INFO.P = glm::perspective(glm::radians(65.0f), (float)width / height, 0.1f, 1000.0f);


	volkInitialize();
	glslang::InitializeProcess();
	SDL_Init(SDL_INIT_EVERYTHING);

	{
		uint32_t extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
		std::vector<VkExtensionProperties> instance_extension(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, instance_extension.data());

		for (auto [i, c] : std::ranges::views::enumerate(instance_extension)) {
			std::println(std::cout, "{} : {}", i, c.extensionName);
		}
	}

	std::vector<const char*> enable_instance_extension{
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};
	std::vector<const char*> enable_instance_layer{
		"VK_LAYER_KHRONOS_validation"
	};

	std::printf("\n====Instance==============================================================================\n");

	VkApplicationInfo vk_application_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.apiVersion = VK_API_VERSION_1_3
	};

	VkInstance m_vk_instance{};
	VkInstanceCreateInfo vk_instance_create_info{};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.enabledExtensionCount = enable_instance_extension.size();
	vk_instance_create_info.ppEnabledExtensionNames = enable_instance_extension.data();
	vk_instance_create_info.enabledLayerCount = enable_instance_layer.size();
	vk_instance_create_info.ppEnabledLayerNames = enable_instance_layer.data();
	vk_instance_create_info.pApplicationInfo = &vk_application_info;
	{
		auto res = vkCreateInstance(&vk_instance_create_info, nullptr, &m_vk_instance);
		VK_CHECK(res);
		std::printf("instance success");
	}

	volkLoadInstance(m_vk_instance);


	std::printf("\n====Physical Device==============================================================================\n");

	VkPhysicalDevice m_vk_physical_device{}; // GPU
	VkPhysicalDeviceFeatures vk_physical_device_features{};
	{
		uint32_t physical_device_count = 0;
		vkEnumeratePhysicalDevices(m_vk_instance, &physical_device_count, nullptr);
		std::vector<VkPhysicalDevice> vk_physical_devices{ physical_device_count };
		vkEnumeratePhysicalDevices(m_vk_instance, &physical_device_count, vk_physical_devices.data());

		for (auto [i, c] : std::ranges::views::enumerate(vk_physical_devices)) {
			VkPhysicalDeviceProperties vk_physical_device_properties{};
			vkGetPhysicalDeviceProperties(c, &vk_physical_device_properties);
			std::printf("%lld : %s\n", i, vk_physical_device_properties.deviceName);
		}

		m_vk_physical_device = vk_physical_devices[0];
	}

	vkGetPhysicalDeviceFeatures(m_vk_physical_device, &vk_physical_device_features);

	{
		uint32_t physical_device_extension_count = 0;
		vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &physical_device_extension_count, nullptr);
		std::vector<VkExtensionProperties> extension_propertieses{ physical_device_extension_count };
		vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &physical_device_extension_count, extension_propertieses.data());

		for (auto [i, c] : std::ranges::views::enumerate(extension_propertieses)) {
			std::printf("%lld : %s\n", i, c.extensionName);
		}
	}

	std::printf("\n====Device==============================================================================\n");

	std::vector<const char*> enable_device_extension{
		"VK_KHR_swapchain",
		"VK_KHR_get_memory_requirements2",
		"VK_KHR_dedicated_allocation",
		"VK_KHR_bind_memory2"
	};

	VkPhysicalDeviceVulkan11Features shader_features{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = nullptr,
		.storageBuffer16BitAccess = false,
		.uniformAndStorageBuffer16BitAccess = false,
		.storagePushConstant16 = false,
		.storageInputOutput16 = false,
		.multiview = false,
		.multiviewGeometryShader = false,
		.multiviewTessellationShader = false,
		.variablePointersStorageBuffer = false,
		.variablePointers = false,
		.protectedMemory = false,
		.samplerYcbcrConversion = false,
		.shaderDrawParameters = true
	};

	VkPhysicalDeviceDescriptorIndexingFeatures desc_features {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
		.pNext = &shader_features,
		.shaderInputAttachmentArrayDynamicIndexing = true,
		.shaderUniformTexelBufferArrayDynamicIndexing = true,
		.shaderStorageTexelBufferArrayDynamicIndexing = true,
		.shaderUniformBufferArrayNonUniformIndexing = true,
		.shaderSampledImageArrayNonUniformIndexing = true,
		.shaderStorageBufferArrayNonUniformIndexing = true,
		.shaderStorageImageArrayNonUniformIndexing = true,
		.shaderInputAttachmentArrayNonUniformIndexing = true,
		.shaderUniformTexelBufferArrayNonUniformIndexing = true,
		.shaderStorageTexelBufferArrayNonUniformIndexing = true,
		.descriptorBindingUniformBufferUpdateAfterBind = true,
		.descriptorBindingSampledImageUpdateAfterBind = true,
		.descriptorBindingStorageImageUpdateAfterBind = true,
		.descriptorBindingStorageBufferUpdateAfterBind = true,
		.descriptorBindingUniformTexelBufferUpdateAfterBind = true,
		.descriptorBindingStorageTexelBufferUpdateAfterBind = true,
		.descriptorBindingUpdateUnusedWhilePending = true,
		.descriptorBindingPartiallyBound = true,
		.descriptorBindingVariableDescriptorCount = true,
		.runtimeDescriptorArray = true
	};

	

	VkPhysicalDeviceFeatures enable_physical_features{
			.robustBufferAccess = false,
			.fullDrawIndexUint32 = false ,
			.imageCubeArray = false ,
			.independentBlend = false ,
			.geometryShader = false ,
			.tessellationShader = false ,
			.sampleRateShading = false ,
			.dualSrcBlend = false ,
			.logicOp = false ,
			.multiDrawIndirect = false ,
			.drawIndirectFirstInstance = false ,
			.depthClamp = false ,
			.depthBiasClamp = false ,
			.fillModeNonSolid = true , // wireframe
			.depthBounds = false ,
			.wideLines = true ,
			.largePoints = true ,
			.alphaToOne = false ,
			.multiViewport = false ,
			.samplerAnisotropy = true ,
			.textureCompressionETC2 = false ,
			.textureCompressionASTC_LDR = false ,
			.textureCompressionBC = false ,
			.occlusionQueryPrecise = false ,
			.pipelineStatisticsQuery = false ,
			.vertexPipelineStoresAndAtomics = false ,
			.fragmentStoresAndAtomics = false ,
			.shaderTessellationAndGeometryPointSize = false ,
			.shaderImageGatherExtended = false ,
			.shaderStorageImageExtendedFormats = false ,
			.shaderStorageImageMultisample = false ,
			.shaderStorageImageReadWithoutFormat = false ,
			.shaderStorageImageWriteWithoutFormat = false ,
			.shaderUniformBufferArrayDynamicIndexing = false ,
			.shaderSampledImageArrayDynamicIndexing = false ,
			.shaderStorageBufferArrayDynamicIndexing = false ,
			.shaderStorageImageArrayDynamicIndexing = false ,
			.shaderClipDistance = false ,
			.shaderCullDistance = false ,
			.shaderFloat64 = false ,
			.shaderInt64 = false ,
			.shaderInt16 = false ,
			.shaderResourceResidency = false ,
			.shaderResourceMinLod = false ,
			.sparseBinding = false ,
			.sparseResidencyBuffer = false ,
			.sparseResidencyImage2D = false ,
			.sparseResidencyImage3D = false ,
			.sparseResidency2Samples = false ,
			.sparseResidency4Samples = false ,
			.sparseResidency8Samples = false ,
			.sparseResidency16Samples = false ,
			.sparseResidencyAliased = false ,
			.variableMultisampleRate = false ,
			.inheritedQueries = false

	};


	float queue_priorities = { 1.0f };
	VkDeviceQueueCreateInfo vk_device_queue_create_info{};
	vk_device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	vk_device_queue_create_info.pQueuePriorities = &queue_priorities;
	vk_device_queue_create_info.queueCount = 1;
	vk_device_queue_create_info.queueFamilyIndex = 0;

	VkDevice m_vk_device{};
	VkDeviceCreateInfo vk_device_create_info{};
	vk_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vk_device_create_info.enabledExtensionCount = enable_device_extension.size();
	vk_device_create_info.ppEnabledExtensionNames = enable_device_extension.data();
	vk_device_create_info.pEnabledFeatures = &enable_physical_features;
	vk_device_create_info.pQueueCreateInfos = &vk_device_queue_create_info;
	vk_device_create_info.queueCreateInfoCount = 1;
	vk_device_create_info.pNext = &desc_features;

	{
		auto res = vkCreateDevice(m_vk_physical_device, &vk_device_create_info, nullptr, &m_vk_device);
		VK_CHECK(res);
		std::printf("device success");
	}
	volkLoadDevice(m_vk_device);

	std::printf("\n====GetQueue==============================================================================\n");

	VkQueue m_vk_queue{}; // for graphics, present and transfer
	vkGetDeviceQueue(m_vk_device, 0, 0, &m_vk_queue);
	std::printf("get queue success");


	std::printf("====A New Window==============================================================================\n");

	SDL_Window* window = SDL_CreateWindow("Hello Warp", width, height, SDL_WINDOW_VULKAN);
	SDL_ShowWindow(window);
	std::printf("create window success");

	std::printf("\n===Surface==============================================================================\n");

	VkSurfaceKHR m_vk_surface{};
	{
		if (!SDL_Vulkan_CreateSurface(window, m_vk_instance, nullptr, &m_vk_surface)) {
			__debugbreak();
		}
	}
	std::printf("create surface success");


	//vma
	std::printf("\n====vma==============================================================================\n");

	VmaAllocator vma_allocator{};
	VmaVulkanFunctions vma_vulkan_func{};
	vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
	vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
	vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
	vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
	vma_vulkan_func.vkCreateImage = vkCreateImage;
	vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
	vma_vulkan_func.vkDestroyImage = vkDestroyImage;
	vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vma_vulkan_func.vkFreeMemory = vkFreeMemory;
	vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
	vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vma_vulkan_func.vkMapMemory = vkMapMemory;
	vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
	vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;
	vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
	vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
	vma_vulkan_func.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
	vma_vulkan_func.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
	vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
	vma_vulkan_func.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
	vma_vulkan_func.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;

	VmaAllocatorCreateInfo vma_allocator_create_info{};
	vma_allocator_create_info.device = m_vk_device;
	vma_allocator_create_info.physicalDevice = m_vk_physical_device;
	vma_allocator_create_info.instance = m_vk_instance;
	vma_allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_3;
	vma_allocator_create_info.pVulkanFunctions = &vma_vulkan_func;

	{
		auto res = vmaCreateAllocator(&vma_allocator_create_info, &vma_allocator);
		VK_CHECK(res);
	}

	std::printf("\n====SwapChain==============================================================================\n");

	VkSwapchainKHR m_vk_swapchain{};
	VkSwapchainCreateInfoKHR vk_swapchain_create_info{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = m_vk_surface,
		.minImageCount = 3,
		.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
		.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = VkExtent2D{ width, height },
		.imageArrayLayers = 1,
		.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = true,
	};

	{
		auto dres = vkCreateSwapchainKHR(m_vk_device, &vk_swapchain_create_info, nullptr, &m_vk_swapchain);
		VK_CHECK(dres);
	}

	std::printf("create swapchain success");

	std::printf("\n====ImagesFromSwapChain==============================================================================\n");

	uint32_t swap_chain_image_count{};

	vkGetSwapchainImagesKHR(m_vk_device, m_vk_swapchain, &swap_chain_image_count, nullptr);
	std::vector<VkImage> vk_swapchain_image{ swap_chain_image_count };
	vkGetSwapchainImagesKHR(m_vk_device, m_vk_swapchain, &swap_chain_image_count, vk_swapchain_image.data());

	printf("swapchain image count : %d\n", swap_chain_image_count);

	if (swap_chain_image_count == 0) {
		__debugbreak();
	}


	std::vector<VkImageView> vk_swapchain_image_view{ swap_chain_image_count };

	for (int i = 0; i < swap_chain_image_count; i++) {
		VkImageViewCreateInfo vk_image_view_create_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = vk_swapchain_image[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		{
			auto res = vkCreateImageView(m_vk_device, &vk_image_view_create_info, nullptr, &vk_swapchain_image_view[i]);
			VK_CHECK(res);
		}
	}

	std::printf("Get SwapChain Images and create Image View success");


	std::printf("\n====prepare for depth buffer==============================================================================\n");


	VkImage depth_image{};
	VmaAllocation depth_image_allocation{};
	VkImageView depth_image_view{};
	{

		VkImageCreateInfo imageci{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.extent = {width, height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		VmaAllocationCreateInfo allocation_info{
			.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		};

		if (VK_SUCCESS != vmaCreateImage(vma_allocator, &imageci, &allocation_info, &depth_image, &depth_image_allocation, nullptr))
		{
			__debugbreak();
		}
		VkImageViewCreateInfo image_view_ci{

				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = depth_image,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
		};

		if(VK_SUCCESS != vkCreateImageView(m_vk_device, &image_view_ci, nullptr, &depth_image_view))
		{
			__debugbreak();
		}
	}


	VkImage depth_image_msaa{};
	VmaAllocation depth_image_msaa_allocation{};
	VkImageView depth_image_msaa_view{};
	{

		VkImageCreateInfo imageci{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.extent = {width, height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = multisample_setting,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		VmaAllocationCreateInfo allocation_info{
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		};

		if (VK_SUCCESS != vmaCreateImage(vma_allocator, &imageci, &allocation_info, &depth_image_msaa, &depth_image_msaa_allocation, nullptr)) {
			__debugbreak();
		}
		VkImageViewCreateInfo image_view_ci{

				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = depth_image_msaa,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
		};

		if (VK_SUCCESS != vkCreateImageView(m_vk_device, &image_view_ci, nullptr, &depth_image_msaa_view)) {
			__debugbreak();
		}
	}


	std::printf("\n====prepare for multisample==============================================================================\n");

	VkImage vk_image_for_multisample{};
	VkImageView vk_image_view_for_multisample{};
	VkDeviceMemory vk_device_memory_for_multisample{};
	VmaAllocation vma_allocation_for_multisample{};

	{
		VkImageCreateInfo vk_image_create_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.extent = VkExtent3D{width, height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = multisample_setting,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		auto res = vkCreateImage(m_vk_device, &vk_image_create_info, nullptr, &vk_image_for_multisample);
		VK_CHECK(res);
		VmaAllocationCreateInfo allocation_info{};
		allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		res = vmaCreateImage(vma_allocator, &vk_image_create_info, &allocation_info, &vk_image_for_multisample, &vma_allocation_for_multisample, nullptr);
		VK_CHECK(res);



		VkImageViewCreateInfo vk_image_view_create_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = vk_image_for_multisample,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
		};
		res = vkCreateImageView(m_vk_device, &vk_image_view_create_info, nullptr, &vk_image_view_for_multisample);
		VK_CHECK(res);


		std::printf("Create imd image for multisample success.");

	}

	std::printf("\n====prepare matcap image==============================================================================\n");

	int matcap_x, matcap_y, z;
	uint8_t* idata = stbi_load("D:/matcap.png", &matcap_x, &matcap_y, &z, STBI_rgb_alpha);
	int mat_cap_size = matcap_x * matcap_y * 4;

	println("{} x {} x {}.", matcap_x, matcap_y, z);

	VkImage image_matcap{};
	VkImageView image_view_matcap{};
	VmaAllocation image_matcap_alloc{};

	VkBuffer buffer_matcap{};
	VmaAllocation buffer_matcap_alloc{};

	{
		VkBufferCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = (uint32_t)mat_cap_size,
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		VmaAllocationCreateInfo buffer_memroy_ci = {
				.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
				.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		};

		if(VK_SUCCESS != vmaCreateBuffer(vma_allocator, &info, &buffer_memroy_ci, &buffer_matcap, &buffer_matcap_alloc, nullptr))
		{
			__debugbreak();
		}

		void* mapper_matcap;
		vmaMapMemory(vma_allocator, buffer_matcap_alloc, &mapper_matcap);
		memcpy(mapper_matcap, idata, mat_cap_size);

		VkImageCreateInfo imgci{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_UNORM,
				.extent = {(uint32_t)matcap_x, (uint32_t)matcap_y, 1},
				.mipLevels = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
				.initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		};
		VmaAllocationCreateInfo vma_alloc_info{
			.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		};

		if (VK_SUCCESS != vmaCreateImage(vma_allocator, &imgci, &vma_alloc_info, &image_matcap, &image_matcap_alloc, nullptr)) {
			__debugbreak();
		}

		VkImageViewCreateInfo image_view_ci{

			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image_matcap,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.components = {.r = VK_COMPONENT_SWIZZLE_R, .g = VK_COMPONENT_SWIZZLE_G, .b = VK_COMPONENT_SWIZZLE_B, .a =VK_COMPONENT_SWIZZLE_A },
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};


		if(VK_SUCCESS != vkCreateImageView(m_vk_device, &image_view_ci, nullptr, &image_view_matcap))
		{
			__debugbreak();
		}
	}

	stbi_image_free(idata);

	std::printf("\n====RenderPass==============================================================================\n");


	VkAttachmentDescription vk_attachment_description{ // render image with multisample
		.format = VK_FORMAT_R8G8B8A8_SRGB,
		.samples = multisample_setting,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription vk_attachment_description2{ // resolve image from multisample image
		.format = VK_FORMAT_R8G8B8A8_SRGB,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentDescription vk_attachment_description3{ // resolve image from multisample image
		.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.samples = multisample_setting,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription vk_attachment_description4{ // resolve image from multisample image
		.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};


	VkAttachmentReference vk_attachment_reference{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference vk_attachment_reference2{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference vk_attachment_reference3{
		.attachment = 2,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference vk_attachment_reference4{
		.attachment = 3,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};


	VkAttachmentReference resolve[] = {
		vk_attachment_reference2,
		vk_attachment_reference4
	};

	VkSubpassDescription vk_subpass_description{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &vk_attachment_reference,
			.pResolveAttachments = resolve,
			.pDepthStencilAttachment = &vk_attachment_reference3,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
	};

	VkRenderPass renderPass{};
	VkAttachmentDescription vk_attachment_description_bind[] = {
		vk_attachment_description,
		vk_attachment_description2,
		vk_attachment_description3,
		vk_attachment_description4
	};

	VkRenderPassCreateInfo vk_render_pass_create_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 4,
		.pAttachments = vk_attachment_description_bind,
		.subpassCount = 1,
		.pSubpasses = &vk_subpass_description,
		.dependencyCount = 0,
		.pDependencies = nullptr
	};

	{
		auto res = vkCreateRenderPass(m_vk_device, &vk_render_pass_create_info, nullptr, &renderPass);
		VK_CHECK(res);
		std::printf("Create Renderpass success");
	}

	std::printf("\n====Read extern resrouce==============================================================================\n");
	tinyobj::attrib_t attrib{};
	std::vector<tinyobj::shape_t> shape{};
	std::vector<tinyobj::material_t> mats{};


	if (tinyobj::LoadObj(&attrib, &shape, &mats, nullptr, "D:/box.obj")) {
		println("load obj success");
	}


	struct alignas(16) ObjVertexInfo {
		float pos[3];
		float normal[3];
		float uv[2];
	};

	std::vector<ObjVertexInfo> vertex_info{};
	std::vector<uint32_t> index_info{};
	vertex_info.reserve(81920);
	index_info.reserve(81920);

	println("vertex : has {} ", attrib.vertices.size());
	println("normals : has {} ", attrib.normals.size());
	println("texcoords : has {} ", attrib.texcoords.size());

	//for(int i = 0; i < attrib.vertices.size() / 3; i ++)
	//{
	//	vertex_info.push_back({
	//		.pos = {
	//			attrib.vertices[3 * i + 0],
	//			attrib.vertices[3 * i + 1],
	//			attrib.vertices[3 * i + 2]
	//		},
	//		.normal = {
	//			attrib.normals[3 * i + 0],
	//			attrib.normals[3 * i + 1],
	//			attrib.normals[3 * i + 2],
	//		},
	//		.uv = {
	//			attrib.texcoords[2 * i + 0],
	//			attrib.texcoords[2 * i + 1]
	//		}
	//	});
	//	//println("vertex : {} {} {}", attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
	//}

	/*for (auto& i : shape) {
		for (auto& j : i.mesh.indices) {
			index_info.push_back(j.vertex_index);
		}
	}*/

	println("vertex : has {} ", vertex_info.size());
	println("index : has {} ", index_info.size());

	
	//shape[0].mesh.
	for (auto& i : shape) {
		for (auto& j : i.mesh.indices) {
			vertex_info.push_back({
				.pos = {
					/*attrib.texcoords[2 * j.texcoord_index + 0],
					attrib.texcoords[2 * j.texcoord_index + 1],
					0,*/

					attrib.vertices[3 * j.vertex_index + 0],
					attrib.vertices[3 * j.vertex_index + 1],
					attrib.vertices[3 * j.vertex_index + 2]
				},
				.normal = {
					//attrib.normals[3 * j.normal_index + 0],

					attrib.normals[3 * j.normal_index + 0],
					attrib.normals[3 * j.normal_index + 1],
					attrib.normals[3 * j.normal_index + 2],
				},
				.uv = {
					attrib.texcoords[2 * j.texcoord_index + 0],
					attrib.texcoords[2 * j.texcoord_index + 1]
				}
			});
			index_info.push_back(index_info.size());
		}
	}


	VkBuffer index_buffer_GPU{};
	VkBufferCreateInfo index_buffer_GPU_create_info{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = sizeof(uint32_t) * index_info.size(),
				.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
	};
	VmaAllocationCreateInfo index_buffer_GPU_alloc_info{
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};
	VmaAllocation index_memory_GPU{};
	if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &index_buffer_GPU_create_info, &index_buffer_GPU_alloc_info, &index_buffer_GPU, &index_memory_GPU, nullptr)) {
		__debugbreak();
	}

	VkBuffer index_buffer{};
	VkBufferCreateInfo index_buffer_create_info{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = sizeof(uint32_t) * index_info.size(),
				.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	};
	VmaAllocationCreateInfo index_buffer_alloc_info{
		.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};
	VmaAllocation index_memory{};
	if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &index_buffer_create_info, &index_buffer_alloc_info, &index_buffer, &index_memory, nullptr)) {
		__debugbreak();
	}


	println("index buffer index_memory");
	void* indexp = nullptr;
	vmaMapMemory(vma_allocator, index_memory, &indexp);

	memcpy(indexp, index_info.data(), sizeof(uint32_t) * index_info.size());


	println("index copy success");


	VkBuffer vertex_buffer{};
	VkBufferCreateInfo vertex_buffer_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = sizeof(ObjVertexInfo) * vertex_info.size(),
		.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	};
	VmaAllocationCreateInfo vertex_buffer_alloc_info{
		.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};



	VkBuffer vertex_buffer_GPU{};
	VkBufferCreateInfo vertex_buffer_GPU_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = sizeof(ObjVertexInfo) * vertex_info.size(),
		.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT
	};
	VmaAllocationCreateInfo vertex_buffer_GPU_alloc_info{
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};

	VmaAllocation vertex_memory{};
	VmaAllocation vertex_memory_GPU{};

	if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &vertex_buffer_create_info, &vertex_buffer_alloc_info, &vertex_buffer, &vertex_memory, nullptr)) {
		__debugbreak();
	}
	if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &vertex_buffer_GPU_create_info, &vertex_buffer_GPU_alloc_info, &vertex_buffer_GPU, &vertex_memory_GPU, nullptr)) {
		__debugbreak();
	}

	println("vertex buffer created");

	void* vertp = nullptr;
	vmaMapMemory(vma_allocator, vertex_memory, &vertp);

	memcpy(vertp, vertex_info.data(), sizeof(ObjVertexInfo) * vertex_info.size());


	println("vertex copy success");

	VkVertexInputBindingDescription vertex_input_binding_description{
		.binding = 0,
		.stride = sizeof(ObjVertexInfo),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	VkVertexInputAttributeDescription vertex_input_attribute_description[3]{
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(ObjVertexInfo, pos)
		},
		{
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(ObjVertexInfo, normal)
		},
		{
			.location = 2,
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(ObjVertexInfo, uv)
		}
	};

	std::printf("\n====Samper create==============================================================================\n");
	kl,.
	VkSampler m_vk_sampler{};
	VkSamplerCreateInfo samplerci{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.anisotropyEnable = true,
			.maxAnisotropy = 8,
			.compareEnable = true,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
	};

	if(VK_SUCCESS != vkCreateSampler(m_vk_device, &samplerci, nullptr, &m_vk_sampler)) {
		__debugbreak();
	}


	std::printf("\n====Descriptor pool==============================================================================\n");

	struct ubo {
		float time;
	};

	struct camera_info {
		glm::mat4 V;
		glm::mat4 P;
	};

	struct model_info {
		glm::mat4 M;
	};

	//ubo, likes time

	VkBuffer vk_buffer_ubo{};
	VmaAllocation vk_buffer_ubo_memory{};
	VkBufferCreateInfo vk_buffer_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = sizeof(ubo),
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};
	VmaAllocationCreateInfo buffer_create_info{
		.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
		.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = nullptr,
		.pUserData = nullptr,
		.priority = 0
	};

	if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &vk_buffer_create_info, &buffer_create_info, &vk_buffer_ubo, &vk_buffer_ubo_memory, nullptr)) {
		__debugbreak();
	}
	printf("buffer create success");

	void* p_data = nullptr;
	vmaMapMemory(vma_allocator, vk_buffer_ubo_memory, &p_data);


	//camera info

	VkBuffer vk_buffer_camera{};
	VmaAllocation vk_buffer_camera_memory{};
	void* mapper_camera{};
	{

		VkBufferCreateInfo createinfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = sizeof(camera_info),
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		};

		VmaAllocationCreateInfo buffer_alloc{
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &createinfo, &buffer_alloc, &vk_buffer_camera, &vk_buffer_camera_memory, nullptr)) {
			__debugbreak();
		}
		printf("camera buffer create success");

		vmaMapMemory(vma_allocator, vk_buffer_camera_memory, &mapper_camera);
	}

	VkBuffer vk_buffer_model{};
	VmaAllocation vk_buffer_model_memory{};

	void* mapper_model{};
	{
		VkBufferCreateInfo createinfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = sizeof(model_info),
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		};

		VmaAllocationCreateInfo buffer_alloc{
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		if (VK_SUCCESS != vmaCreateBuffer(vma_allocator, &createinfo, &buffer_alloc, &vk_buffer_model, &vk_buffer_model_memory, nullptr)) {
			__debugbreak();
		}
		printf("model buffer create success");

		vmaMapMemory(vma_allocator, vk_buffer_model_memory, &mapper_model);
	}

	model_info model_data{};
	model_data.M = glm::mat4(1.0f);

	//glm::rotate(model_data.M, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	camera_info camera_data{};
	camera_data.V = glm::lookAt(glm::vec3(0.0f, 3.0f, 3.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0, -1.0f, 0.0f));
	camera_data.P = glm::perspective(glm::radians(46.7f), (float)width / height, 0.1f, 1000.0f);


	memcpy(mapper_camera, &camera_data, sizeof(camera_info));
	memcpy(mapper_model, &model_data, sizeof(model_data));


	//desc:

	VkDescriptorPool m_vk_descriptor_pool{};

	VkDescriptorSetLayoutBinding binding_info[4]{
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
		},
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
		},
		{
			.binding = 2,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
		},
		{
			.binding = 3,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = nullptr
		}
	};

	//buffer0 for CPU
	VkBuffer good_buffer{};
	VmaAllocation go_allocP{};
	VkBufferCreateInfo good_buffer_ci{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = sizeof(objInfo),
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};

	VmaAllocationCreateInfo good_buffer_alloc_info{
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	vmaCreateBuffer(vma_allocator, &good_buffer_ci, &good_buffer_alloc_info, &good_buffer, &go_allocP, nullptr);
	void* pm = nullptr;
	vmaMapMemory(vma_allocator, go_allocP, &pm);
	memcpy(pm, &OBJ_INFO, sizeof(objInfo));

	//buffer0 for GPU
	VkBuffer good_buffer_gpu{};
	VmaAllocation go_alloc_gpu{};
	VkBufferCreateInfo good_buffer_gpu_ci{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = sizeof(objInfo),
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};

	VmaAllocationCreateInfo good_buffer_gpu_alloc_info{
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};

	vmaCreateBuffer(vma_allocator, &good_buffer_gpu_ci, &good_buffer_gpu_alloc_info, &good_buffer_gpu, &go_alloc_gpu, nullptr);



	//Pool

	VkDescriptorPool good_desc_pool{};

	std::array size = {
		VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,.descriptorCount = 8192 * 3},
		VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.descriptorCount = 8192 },
	};

	VkDescriptorPoolCreateInfo descpool_ci{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 32,
		.poolSizeCount = size.size(),
		.pPoolSizes = size.data()
	};

	vkCreateDescriptorPool(m_vk_device, &descpool_ci, nullptr, &good_desc_pool);


	//FLAGS
	VkDescriptorBindingFlags setlayout_flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

	std::vector setlayout_flags(1, setlayout_flag);

	VkDescriptorSetLayoutBindingFlagsCreateInfo setlayout_flag_ci{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.pNext = nullptr,
		.bindingCount = static_cast<uint32_t>(setlayout_flags.size()),
		.pBindingFlags = setlayout_flags.data()
	};

	//Image setlayout


	std::array global_image_binding{
		VkDescriptorSetLayoutBinding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8192, VkShaderStageFlagBits::VK_SHADER_STAGE_ALL, nullptr},
	};

	VkDescriptorSetLayoutCreateInfo global_image_setlayout_ci {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &setlayout_flag_ci,
		.flags = 0,
		.bindingCount = global_image_binding.size(),
		.pBindings = global_image_binding.data()
	};

	VkDescriptorSetLayout global_image_setlayout{};

	if(vkCreateDescriptorSetLayout(m_vk_device, &global_image_setlayout_ci, nullptr, &global_image_setlayout) == VK_SUCCESS) {
		println("global_image_setlayout set layout create success");
	}


	//buffers setlayout
	std::array global_buffer_binding{
		VkDescriptorSetLayoutBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8192, VkShaderStageFlagBits::VK_SHADER_STAGE_ALL, nullptr},
	};

	VkDescriptorSetLayoutCreateInfo global_buffer_set_layout_ci{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &setlayout_flag_ci,
		.flags = 0,
		.bindingCount = global_buffer_binding.size(),
		.pBindings = global_buffer_binding.data()
	};
	VkDescriptorSetLayout global_buffer_setlayout{};

	if (vkCreateDescriptorSetLayout(m_vk_device, &global_buffer_set_layout_ci, nullptr, &global_buffer_setlayout) == VK_SUCCESS) {
		println("global_buffer_set_layout_ci set layout create success");
	}



	//alloc

	VkDescriptorSetAllocateInfo global_set_alloc{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = good_desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &global_image_setlayout
	};

	VkDescriptorSet images_set{};
	if(VK_SUCCESS == vkAllocateDescriptorSets(m_vk_device, &global_set_alloc, &images_set)) {
		println("images_set allocate set success");
	}


	// 0: global, 1: material, 2: per object
	VkDescriptorSet buffers_set[3]{};

	VkDescriptorSetLayout layouts[3] = {
		global_buffer_setlayout,
		global_buffer_setlayout,
		global_buffer_setlayout
	};

	global_set_alloc = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = good_desc_pool,
		.descriptorSetCount = 3,
		.pSetLayouts = layouts
	};

	if (VK_SUCCESS == vkAllocateDescriptorSets(m_vk_device, &global_set_alloc, buffers_set)) {
		println("buffers_set 3 allocate set success");
	}

	//write

	VkDescriptorImageInfo image_info{
		.sampler = m_vk_sampler,
		.imageView = image_view_matcap,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkDescriptorBufferInfo buf_info{
		.buffer = good_buffer_gpu,
		.offset = 0,
		.range = sizeof(objInfo)
	};

	std::vector<VkWriteDescriptorSet> img_write_set{
		{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = nullptr,
		.dstSet = buffers_set[0],
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.pImageInfo = nullptr,
		.pBufferInfo = &buf_info,
		.pTexelBufferView = nullptr
		},
		{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = nullptr,
		.dstSet = images_set,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo = &image_info,
		.pBufferInfo = nullptr,
		.pTexelBufferView = nullptr
		},
	};

	vkUpdateDescriptorSets(m_vk_device, img_write_set.size(), img_write_set.data(), 0, nullptr);
	


	//VkDescriptorSetLayoutCreateInfo m_vk_descriptor_set_layout_create_info{
	//	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
	//	.pNext = nullptr,
	//	.flags = 0,
	//	.bindingCount = 4,
	//	.pBindings = binding_info
	//};

	//VkDescriptorSetLayout m_vk_set_layout{};
	//if (VK_SUCCESS != vkCreateDescriptorSetLayout(m_vk_device, &m_vk_descriptor_set_layout_create_info,
	//	nullptr, &m_vk_set_layout)) {
	//	__debugbreak();
	//}

	//VkDescriptorPoolSize pool_size[4]{

	//	{
	//		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//		.descriptorCount = 1
	//	},
	//	{
	//		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//		.descriptorCount = 1
	//	},
	//	{
	//		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//		.descriptorCount = 1
	//	},
	//	{
	//		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	//		.descriptorCount = 1,
	//	},
	//};


	//VkDescriptorPoolCreateInfo descriptor_pool_create_info{
	//	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
	//	.pNext = nullptr,
	//	.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
	//	.maxSets = 1,
	//	.poolSizeCount = 4,
	//	.pPoolSizes = pool_size
	//};

	//if (VK_SUCCESS != vkCreateDescriptorPool(m_vk_device, &descriptor_pool_create_info, nullptr,
	//	&m_vk_descriptor_pool)) {
	//	__debugbreak();
	//}
	//printf("Descriptor pool create success");


	//VkDescriptorSetAllocateInfo set_alloc_info{
	//	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
	//	.pNext = nullptr,
	//	.descriptorPool = m_vk_descriptor_pool,
	//	.descriptorSetCount = 1,
	//	.pSetLayouts = &m_vk_set_layout
	//};

	//VkDescriptorSet one_set{};


	//vkAllocateDescriptorSets(m_vk_device, &set_alloc_info, &one_set);


	//VkDescriptorBufferInfo buffer_info_ubo{
	//	.buffer = vk_buffer_ubo,
	//	.range = sizeof(ubo)
	//};

	//VkDescriptorBufferInfo buffer_info_camera{
	//	.buffer = vk_buffer_camera,
	//	.range = sizeof(camera_info)
	//};

	//VkDescriptorBufferInfo buffer_info_model{
	//	.buffer = vk_buffer_model,
	//	.range = sizeof(model_info)
	//};

	//VkDescriptorImageInfo matcap_image{
	//	.sampler = m_vk_sampler,
	//	.imageView = image_view_matcap,
	//	.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	//};


	//VkWriteDescriptorSet write_set{
	//	.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//	.dstSet = one_set,
	//	.descriptorCount = 1,
	//	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//	.pBufferInfo = &buffer_info_ubo,
	//};

	//VkWriteDescriptorSet write_set_camera{
	//	.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//	.dstSet = one_set,
	//	.dstBinding = 1,
	//	.descriptorCount = 1,
	//	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//	.pBufferInfo = &buffer_info_camera,
	//};

	//VkWriteDescriptorSet write_set_model{
	//	.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//	.dstSet = one_set,
	//	.dstBinding = 2,
	//	.descriptorCount = 1,
	//	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//	.pBufferInfo = &buffer_info_model,
	//};

	//VkWriteDescriptorSet write_set_sampler {
	//	.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//	.dstSet = one_set,
	//	.dstBinding = 3,
	//	.descriptorCount = 1,
	//	.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	//	.pImageInfo = &matcap_image
	//};

	//ubo myData{ 0.0f };
	//memcpy(p_data, &myData, sizeof(myData));

	//vkUpdateDescriptorSets(m_vk_device, 1, &write_set, 0, nullptr);
	//vkUpdateDescriptorSets(m_vk_device, 1, &write_set_camera, 0, nullptr);
	//vkUpdateDescriptorSets(m_vk_device, 1, &write_set_model, 0, nullptr);
	//vkUpdateDescriptorSets(m_vk_device, 1, &write_set_sampler, 0, nullptr);

	std::printf("\n====Pipeline create==============================================================================\n");
	VkPipeline vk_pipeline{};
	VkGraphicsPipelineCreateInfo vk_graphics_pipeline_create_info{};
	vk_graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	//dynamic state

	VkPipelineDynamicStateCreateInfo vk_pipeline_dynamic_state_create_info{};
	vk_pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	std::vector<VkDynamicState> vk_dynamic_states{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	vk_pipeline_dynamic_state_create_info.pDynamicStates = vk_dynamic_states.data();
	vk_pipeline_dynamic_state_create_info.dynamicStateCount = vk_dynamic_states.size();

	vk_graphics_pipeline_create_info.pDynamicState = &vk_pipeline_dynamic_state_create_info;

	//vk_shader_module
	VkShaderModule vk_shader_module[2]{};

	MVector<uint32_t> src_vert{};
	MVector<uint32_t> src_frag{};

	//auto src_vert = Warp::GPU::ShaderCompiler::compile_shader_from_file("../../../Shader/shader_ubo.vert", EShLangVertex);
	//auto src_frag = Warp::GPU::ShaderCompiler::compile_shader_from_file("../../../Shader/shader_ubo.frag", EShLangFragment);
	MString err = Warp::GPU::ShaderCompiler::compile_shader_from_file("../../../Shader/shader_ubo.vert", "vert", EShLangVertex, src_vert);
	MString err2 = Warp::GPU::ShaderCompiler::compile_shader_from_file("../../../Shader/shader_ubo.frag", "frag", EShLangFragment, src_frag);

	if(!err.empty()){
		println("vert shader compile error: {}", err);
	}

	if(!err2.empty())
	{
		println("frag shader compile error: {}", err2);
	}

	VkShaderModuleCreateInfo vk_shader_module_create_info[2]{};
	vk_shader_module_create_info[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vk_shader_module_create_info[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	vk_shader_module_create_info[0].codeSize = src_vert.size() * sizeof(uint32_t);
	vk_shader_module_create_info[1].codeSize = src_frag.size() * sizeof(uint32_t);

	vk_shader_module_create_info[0].pCode = src_vert.data();
	vk_shader_module_create_info[1].pCode = src_frag.data();

	{
		auto res = vkCreateShaderModule(m_vk_device, &vk_shader_module_create_info[0], nullptr, &vk_shader_module[0]);
		VK_CHECK(res);
		res = vkCreateShaderModule(m_vk_device, &vk_shader_module_create_info[1], nullptr, &vk_shader_module[1]);
		VK_CHECK(res);
	}


	VkPipelineShaderStageCreateInfo vk_pipeline_shader_stage_create_info[2]{};
	vk_pipeline_shader_stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vk_pipeline_shader_stage_create_info[0].pName = "main";
	vk_pipeline_shader_stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	vk_pipeline_shader_stage_create_info[0].module = vk_shader_module[0];

	vk_pipeline_shader_stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vk_pipeline_shader_stage_create_info[1].pName = "main";
	vk_pipeline_shader_stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	vk_pipeline_shader_stage_create_info[1].module = vk_shader_module[1];


	vk_graphics_pipeline_create_info.stageCount = 2;
	vk_graphics_pipeline_create_info.pStages = vk_pipeline_shader_stage_create_info;



	//vk input assembly
	VkPipelineInputAssemblyStateCreateInfo vk_pipeline_input_assembly_state_create_info{};
	vk_pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vk_pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vk_graphics_pipeline_create_info.pInputAssemblyState = &vk_pipeline_input_assembly_state_create_info;

	//vertex input
	VkPipelineVertexInputStateCreateInfo vk_pipeline_vertex_input_state_create_info{};
	vk_pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vk_pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
	vk_pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = &vertex_input_binding_description;
	vk_pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 3;
	vk_pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_input_attribute_description;

	vk_graphics_pipeline_create_info.pVertexInputState = &vk_pipeline_vertex_input_state_create_info;

	//multisample
	VkPipelineMultisampleStateCreateInfo vk_pipeline_multisample_state_create_info{};
	vk_pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vk_pipeline_multisample_state_create_info.rasterizationSamples = multisample_setting;
	vk_pipeline_multisample_state_create_info.minSampleShading = 1.0f;

	vk_graphics_pipeline_create_info.pMultisampleState = &vk_pipeline_multisample_state_create_info;

	//view port

	VkViewport vk_view_port = {
			0, 0, width * 1.0f, height * 1.0f, 0, 1
	};
	VkRect2D vk_scissors{ {0, 0}, {width, height} };

	VkPipelineViewportStateCreateInfo vk_pipeline_viewport_state_create_info{};
	vk_pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vk_pipeline_viewport_state_create_info.viewportCount = 1;
	vk_pipeline_viewport_state_create_info.scissorCount = 1;
	vk_pipeline_viewport_state_create_info.pViewports = &vk_view_port;
	vk_pipeline_viewport_state_create_info.pScissors = &vk_scissors;

	vk_graphics_pipeline_create_info.pViewportState = &vk_pipeline_viewport_state_create_info;


	//depth stencil
	VkPipelineDepthStencilStateCreateInfo vk_pipeline_depth_stencil_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = true,
		.depthWriteEnable = true,
		.depthCompareOp = VK_COMPARE_OP_LESS,
	};

	vk_graphics_pipeline_create_info.pDepthStencilState = &vk_pipeline_depth_stencil_state_create_info;


	//rasterization

	VkPipelineRasterizationStateCreateInfo vk_pipeline_rasterization_state_create_info{};
	vk_pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vk_pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
	vk_pipeline_rasterization_state_create_info.lineWidth = 1.0f;
	vk_pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

	vk_graphics_pipeline_create_info.pRasterizationState = &vk_pipeline_rasterization_state_create_info;


	//Color Blend
	VkPipelineColorBlendAttachmentState vk_pipeline_color_blend_attachment_state{};
	vk_pipeline_color_blend_attachment_state.blendEnable = false;
	vk_pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	vk_pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	vk_pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	vk_pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	vk_pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo vk_pipeline_color_blend_state_create_info{};
	vk_pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vk_pipeline_color_blend_state_create_info.attachmentCount = 1;
	vk_pipeline_color_blend_state_create_info.pAttachments = &vk_pipeline_color_blend_attachment_state;

	vk_graphics_pipeline_create_info.pColorBlendState = &vk_pipeline_color_blend_state_create_info;

	//layout
	VkPipelineLayout vk_pipeline_layout{};

	VkPushConstantRange range = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(objInfo)
	};

	VkDescriptorSetLayout good_set_layouts[4]{
		global_image_setlayout,
		global_buffer_setlayout,
		global_buffer_setlayout,
		global_buffer_setlayout
	};

	VkPipelineLayoutCreateInfo vk_pipeline_layout_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 4,
		.pSetLayouts = good_set_layouts,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr //range
	};


	vk_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	{
		auto res = vkCreatePipelineLayout(m_vk_device, &vk_pipeline_layout_create_info, nullptr, &vk_pipeline_layout);
		VK_CHECK(res);
	}

	vk_graphics_pipeline_create_info.layout = vk_pipeline_layout;

	//render pass
	vk_graphics_pipeline_create_info.renderPass = renderPass;

	{
		auto res = vkCreateGraphicsPipelines(m_vk_device, nullptr, 1, &vk_graphics_pipeline_create_info, nullptr, &vk_pipeline);
		VK_CHECK(res);
		std::printf("Create pipeline success");

	}

	std::printf("\n====create frame buffer==============================================================================\n");


	std::vector<VkFramebuffer> vk_framebuffer{ swap_chain_image_count };


	for (int i = 0; i < swap_chain_image_count; i++) {
		VkFramebufferCreateInfo vk_framebuffer_create_info{};
		vk_framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vk_framebuffer_create_info.attachmentCount = 4;
		VkImageView bind[] = { vk_image_view_for_multisample, vk_swapchain_image_view[i], depth_image_msaa_view, depth_image_view};
		vk_framebuffer_create_info.pAttachments = bind;
		vk_framebuffer_create_info.width = width;
		vk_framebuffer_create_info.height = height;
		vk_framebuffer_create_info.layers = 1;
		vk_framebuffer_create_info.renderPass = renderPass;
		{
			auto res = vkCreateFramebuffer(m_vk_device, &vk_framebuffer_create_info, nullptr, &vk_framebuffer[i]);
			VK_CHECK(res);
		}
	}
	std::printf("Create 3 frame buffer success");

	std::printf("\n====create commmand pool==============================================================================\n");

	VkCommandPool vk_command_pool{};
	VkCommandPoolCreateInfo vk_command_pool_create_info{};
	vk_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vk_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vk_command_pool_create_info.queueFamilyIndex = 0;
	{
		auto res = vkCreateCommandPool(m_vk_device, &vk_command_pool_create_info, nullptr, &vk_command_pool);
		VK_CHECK(res);
		std::printf("Create Command Pool success");
	}

	std::printf("\n====create commmand buffer==============================================================================\n");


	std::vector<VkCommandBuffer> vk_command_buffer{ swap_chain_image_count };
	for (int i = 0; i < swap_chain_image_count; i++) {
		VkCommandBufferAllocateInfo vk_command_buffer_allocate_info{};
		vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vk_command_buffer_allocate_info.commandBufferCount = 1;
		vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vk_command_buffer_allocate_info.commandPool = vk_command_pool;
		{
			auto res = vkAllocateCommandBuffers(m_vk_device, &vk_command_buffer_allocate_info, &vk_command_buffer[i]);
			VK_CHECK(res);
		}
	}

	std::printf("Create 3 Command buffers success");


	std::printf("\n====Create sync object==============================================================================\n");

	std::vector<VkSemaphore> semaphore_image_can_be_render{ swap_chain_image_count };
	std::vector<VkSemaphore> semaphore_image_render_completed{ swap_chain_image_count };
	std::vector<VkFence> image_render_fence{ swap_chain_image_count };

	VkSemaphoreCreateInfo vk_semaphore_create_info{};
	vk_semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo vk_fence_create_info{};
	vk_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vk_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	{
		for (int i = 0; i < swap_chain_image_count; i++) {
			auto res = vkCreateSemaphore(m_vk_device, &vk_semaphore_create_info, nullptr, &semaphore_image_can_be_render[i]);
			VK_CHECK(res);
			res = vkCreateSemaphore(m_vk_device, &vk_semaphore_create_info, nullptr, &semaphore_image_render_completed[i]);
			VK_CHECK(res);
			res = vkCreateFence(m_vk_device, &vk_fence_create_info, nullptr, &image_render_fence[i]);
			VK_CHECK(res);
		}
		std::printf("Create sync object success");

	}


	

	

	println("Draw {} index !!!!!!!!!!!!!!!!!!!", index_info.size());
	println("Draw {} vertex !!!!!!!!!!!!!!!!!!!", vertex_info.size());

	bool first = true;
	int max_record = 0;
	auto rendering_func = [&]() {
		
		static uint32_t current_render_idx = 0;
		static uint32_t image_index = 0;

		//std::printf("\n====Get Next SwapChain image indexc==============================================================================\n");

		vkWaitForFences(m_vk_device, 1, &image_render_fence[current_render_idx], true, UINT64_MAX);
		vkAcquireNextImageKHR(m_vk_device, m_vk_swapchain, UINT64_MAX, semaphore_image_can_be_render[current_render_idx], VK_NULL_HANDLE, &image_index);
		vkResetFences(m_vk_device, 1, &image_render_fence[current_render_idx]);

		//std::printf("\n====Record Command==============================================================================\n");
		if (max_record < swap_chain_image_count + 1) {
			max_record++;
		
			vkResetCommandBuffer(vk_command_buffer[current_render_idx], 0);
			VkCommandBufferBeginInfo vk_command_buffer_begin_info{};
			vk_command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			{
				auto res = vkBeginCommandBuffer(vk_command_buffer[current_render_idx], &vk_command_buffer_begin_info);
				VK_CHECK(res);
			}

			VkRenderPassBeginInfo vk_render_pass_begin_info{};
			vk_render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vk_render_pass_begin_info.clearValueCount = 4;
			vk_render_pass_begin_info.framebuffer = vk_framebuffer[image_index];
			vk_render_pass_begin_info.renderPass = renderPass;
			vk_render_pass_begin_info.renderArea.extent = VkExtent2D{ width, height };
			vk_render_pass_begin_info.renderArea.offset = VkOffset2D{ 0, 0 };

			VkClearValue vk_clear_value{ 0.0f, 0.0f, 0.0f, 1.0f };
			VkClearValue vk_clear_value_depth{.depthStencil = {.depth = 1.0f, .stencil = 0}};
			VkClearValue vk_clear_value_bind[]{ vk_clear_value , vk_clear_value, vk_clear_value_depth, vk_clear_value_depth };

			vk_render_pass_begin_info.pClearValues = vk_clear_value_bind;

			if (first) {

				VkBufferCopy copyRegion1{};
				copyRegion1.size = sizeof(ObjVertexInfo) * vertex_info.size();
				vkCmdCopyBuffer(vk_command_buffer[current_render_idx], vertex_buffer, vertex_buffer_GPU, 1, &copyRegion1);

				VkBufferCopy copyRegion2{};
				copyRegion2.size = sizeof(uint32_t) * index_info.size();
				vkCmdCopyBuffer(vk_command_buffer[current_render_idx], index_buffer, index_buffer_GPU, 1, &copyRegion2);

				VkBufferImageCopy copy_region{
					.bufferOffset = 0,
					.bufferRowLength = 0,
					.bufferImageHeight = 0,
					.imageSubresource = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = 0,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.imageOffset = {0,0,0},
					.imageExtent = {(uint32_t)matcap_x, (uint32_t)matcap_y, 1}
				};

				vkCmdCopyBufferToImage(vk_command_buffer[current_render_idx], buffer_matcap, image_matcap, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);


				first = false;
				VkImageMemoryBarrier bar{};
				bar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				bar.image = image_matcap;
				bar.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				bar.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				bar.subresourceRange.baseArrayLayer = 0;
				bar.subresourceRange.baseMipLevel = 0;
				bar.subresourceRange.layerCount = 1;
				bar.subresourceRange.levelCount = 1;
				bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bar.srcAccessMask = 0;
				bar.dstAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;

				vkCmdPipelineBarrier(vk_command_buffer[current_render_idx], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &bar);
			}

			VkBufferCopy copyRegion3{};
			copyRegion3.size = sizeof(objInfo);
			vkCmdCopyBuffer(vk_command_buffer[current_render_idx], good_buffer, good_buffer_gpu, 1, &copyRegion3);


			vkCmdBeginRenderPass(vk_command_buffer[current_render_idx], &vk_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(vk_command_buffer[current_render_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);


			/*vkCmdPushConstants(vk_command_buffer[current_render_idx], vk_pipeline_layout, 
				VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(objInfo), &OBJ_INFO);*/

			VkDescriptorSet sets[4] = { images_set, buffers_set[0], buffers_set[1], buffers_set[2] };
			vkCmdBindDescriptorSets(vk_command_buffer[current_render_idx], VK_PIPELINE_BIND_POINT_GRAPHICS,
				vk_pipeline_layout, 0, 4, sets,0, nullptr);

			/*vkCmdBindDescriptorSets(vk_command_buffer[current_render_idx], VK_PIPELINE_BIND_POINT_GRAPHICS,
			vk_pipeline_layout, 0, 1, &one_set, 0, nullptr);*/

			vkCmdSetViewport(vk_command_buffer[current_render_idx], 0, 1, &vk_view_port);
			vkCmdSetScissor(vk_command_buffer[current_render_idx], 0, 1, &vk_scissors);

			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(vk_command_buffer[current_render_idx], 0, 1, &vertex_buffer_GPU, &offset);
			vkCmdBindIndexBuffer(vk_command_buffer[current_render_idx], index_buffer_GPU, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDraw(vk_command_buffer[current_render_idx], index_info.size(), 1, 0, 0);

			vkCmdEndRenderPass(vk_command_buffer[current_render_idx]);

			{
				auto res = vkEndCommandBuffer(vk_command_buffer[current_render_idx]);
				VK_CHECK(res);
			}
		}

		//std::printf("\n====Submit Commands==============================================================================\n");

		VkSubmitInfo vk_submit_info{};
		vk_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		vk_submit_info.commandBufferCount = 1;
		vk_submit_info.pCommandBuffers = &vk_command_buffer[current_render_idx];

		VkSemaphore waiting_semaphores[] = { semaphore_image_can_be_render[current_render_idx] }; // 等待的semahpore
		vk_submit_info.pWaitSemaphores = waiting_semaphores;
		vk_submit_info.waitSemaphoreCount = 1;

		VkPipelineStageFlags waitS_stage[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // 要等待的阶段
		vk_submit_info.pWaitDstStageMask = waitS_stage;

		VkSemaphore finish_semaphores[] = { semaphore_image_render_completed[current_render_idx] }; // 渲染的semahpore
		vk_submit_info.pSignalSemaphores = finish_semaphores;
		vk_submit_info.signalSemaphoreCount = 1;

		{
			auto res = vkQueueSubmit(m_vk_queue, 1, &vk_submit_info, image_render_fence[current_render_idx]);
			VK_CHECK(res);
		}

		//std::printf("\n====present image==============================================================================\n");

		VkPresentInfoKHR vk_present_info{};
		vk_present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		vk_present_info.waitSemaphoreCount = 1;
		vk_present_info.pWaitSemaphores = finish_semaphores;
		vk_present_info.pImageIndices = &image_index;
		vk_present_info.pSwapchains = &m_vk_swapchain;
		vk_present_info.swapchainCount = 1;

		{
			auto res = vkQueuePresentKHR(m_vk_queue, &vk_present_info);
			if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
				//spdlog::info("调整窗口大小");
			} else if (res != VK_SUCCESS) {
				//spdlog::error("获取下一张图像失败");
				return;
			}
		}
		current_render_idx = (current_render_idx + 1) % swap_chain_image_count;

		};
	
	
	
	float moveX = 0.0f;
	float moveY = 0.0f;
	bool relta_mouse_mode = false;
	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));

	auto logic_func = [&](const SDL_Event& sevent) {

		if (sevent.type == SDL_EVENT_KEY_DOWN) {
			switch (sevent.key.keysym.sym) {
			
			}
		}else if (sevent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			if (sevent.button.button == SDL_BUTTON_MIDDLE) {
				SDL_HideCursor();
				//SDL_SetRelativeMouseMode(SDL_TRUE);
				relta_mouse_mode = true;
			}
			
		}else if (sevent.type == SDL_EVENT_MOUSE_BUTTON_UP) {
			if (sevent.button.button == SDL_BUTTON_MIDDLE) {
				//SDL_SetRelativeMouseMode(SDL_FALSE);
				SDL_ShowCursor();
				relta_mouse_mode = false;
			}
		} else if (sevent.type == SDL_EVENT_MOUSE_MOTION) {
			if (relta_mouse_mode) {

				moveX += sevent.motion.xrel;
				moveY += sevent.motion.yrel;

				if (moveY > 360) {
					moveY -= 360;
				} else if (moveY < -360) {
					moveY += 360;
				}

				auto yRot = glm::angleAxis(glm::radians(moveX), glm::vec3(0.0f, 1.0f, 0.0f));
				auto xRot = glm::angleAxis(glm::radians(-moveY), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat final_rot = glm::normalize(yRot * xRot);
				glm::mat4 final_rot_mat = glm::mat4_cast(final_rot);

				glm::vec4 eyePos = glm::vec4(0.0f, 0.0f, 6.0f, 1.0f);

				glm::vec3 eye = final_rot_mat * eyePos;
				glm::vec3 up = (final_rot_mat * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));

				OBJ_INFO.V = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), up);

				OBJ_INFO.P = glm::perspective(glm::radians(46.7f), (float)width / height, 0.1f, 1000.0f);

				memcpy(pm, &OBJ_INFO, sizeof(objInfo));
			}
		}
		
	};
	std::printf("\n====loop==============================================================================\n");
	//sdl
	SDL_Event event{};
	bool should_close = false;
	
	size_t now_time = SDL_GetTicks();
	size_t fpstime = now_time;
	uint32_t fps = 0;

	while (!should_close) {
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) {
			should_close = true;
		}
		logic_func(event);
		rendering_func();


		now_time = SDL_GetTicks();
		if(now_time - fpstime > 1000)
		{
			fpstime = now_time;
			std::string time = std::format("hello fps: {}", fps);
			SDL_SetWindowTitle(window, time.c_str());
			fps = 0;
		}
		fps ++;
	}

	std::printf("\n====end and clean==============================================================================\n");

	vkDeviceWaitIdle(m_vk_device);
	for (int i = 0; i < swap_chain_image_count; i++) {
		vkDestroySemaphore(m_vk_device, semaphore_image_can_be_render[i], nullptr);
		vkDestroySemaphore(m_vk_device, semaphore_image_render_completed[i], nullptr);
		vkDestroyFence(m_vk_device, image_render_fence[i], nullptr);
	}

	vkDestroyCommandPool(m_vk_device, vk_command_pool, nullptr);

	for (int i = 0; i < swap_chain_image_count; i++) {
		vkDestroyFramebuffer(m_vk_device, vk_framebuffer[i], nullptr);
	}

	vkDestroyRenderPass(m_vk_device, renderPass, nullptr);
	for (int i = 0; i < swap_chain_image_count; i++) {
		vkDestroyImageView(m_vk_device, vk_swapchain_image_view[i], nullptr);
	}

	vkDestroyShaderModule(m_vk_device, vk_shader_module[0], nullptr);
	vkDestroyShaderModule(m_vk_device, vk_shader_module[1], nullptr);
	vkDestroyPipeline(m_vk_device, vk_pipeline, nullptr);
	vkDestroyPipelineLayout(m_vk_device, vk_pipeline_layout, nullptr);
	vkDestroySwapchainKHR(m_vk_device, m_vk_swapchain, nullptr);
	vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);
	vkDestroyDevice(m_vk_device, nullptr);
	vkDestroyInstance(m_vk_instance, nullptr);

	SDL_DestroyWindow(window);

	return 0;
}