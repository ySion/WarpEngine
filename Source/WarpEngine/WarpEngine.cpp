#include "WarpEngine.hpp"
#include "Loaders/tiny_obj_loader.h"
#include "windows.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"

#undef ERROR
namespace Warp {

	Render::Render() : keyboardId(manager.CreateDevice<gainput::InputDeviceKeyboard>()), map(manager) {

		spdlog::info("WarpEngine::Engine()");
	}
	Render::~Render() {
		spdlog::info("WarpEngine::~Engine()");
	}

	void Render::run() {
		spdlog::info("WarpEngine::run()");

		glslang_initialize_process();

		optional<bool> result = init_window()
			.and_then([&](optional<bool>) { return create_instance(); })
			.and_then([&](optional<bool>) { return init_vulkan_debug_message(); })
			.and_then([&](optional<bool>) { return create_surface(); })
			.and_then([&](optional<bool>) { return select_physical_device(); })
			.and_then([&](optional<bool>) { return create_logical_device_and_queue(); })
			.and_then([&](optional<bool>) { return create_command_pool(); })
			.and_then([&](optional<bool>) { return create_swap_chain(); })
			.and_then([&](optional<bool>) {
			create_depth_image();
			create_test_image();
			return create_swap_chain_image_view();
				})
			.and_then([&](optional<bool>) { return create_render_pass(); })
					.and_then([&](optional<bool>) { return create_descriptor_set_layout(); })
					.and_then([&](optional<bool>) { return create_graphics_pipeline(); })
					.and_then([&](optional<bool>) { return create_framebuffers(); })
					.and_then([&](optional<bool>) { return create_test_model(); })
					.and_then([&](optional<bool>) { return create_vertex_buffer(); })
					.and_then([&](optional<bool>) { return create_index_buffer(); })
					.and_then([&](optional<bool>) { return create_uniform_buffer(); })
					.and_then([&](optional<bool>) { return create_descriptor_pool(); })
					.and_then([&](optional<bool>) { return create_descriptor_sets(); })
					.and_then([&](optional<bool>) { return create_command_buffer(); })
					.and_then([&](optional<bool>) { return create_semaphores(); });

				if (result) {
					main_loop();

					end();
				}
	}

	optional<bool> Render::init_window() {

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 窗口不能变动

		window = glfwCreateWindow(windowWidth, windowHeight, "WarpEngine", nullptr, nullptr);
		map.MapBool(BT_A, keyboardId, gainput::KeyA);
		map.MapBool(BT_D, keyboardId, gainput::KeyD);
		manager.SetDisplaySize(windowWidth, windowHeight);
		if (window == nullptr) {
			spdlog::error("创建窗口失败");
			return std::nullopt;
		}
		glfwShowWindow(window);
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeLimits(window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		return true;
	}

	optional<bool> Render::create_instance() {
		VkApplicationInfo info{};
		info.apiVersion = VK_API_VERSION_1_3;
		info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		info.pApplicationName = "WarpEngine";
		info.pEngineName = "WarpEngine";
		info.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;

		VkInstanceCreateInfo createInfo{};
		createInfo.pApplicationInfo = &info;
		createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		if (enableValidationLayers) {
			enableInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			enableLayers.push_back("VK_LAYER_KHRONOS_validation");
		}

		vector<const char*> extensionsName{};

		vector<const char*> layersName{};
		auto                exts = check_extensions();
		if (exts) {
			auto& extensions = exts.value();
			for (const auto& extension : extensions) {
				for (const auto& enableExtension : enableInstanceExtensions) {
					if (strcmp(extension.extensionName, enableExtension) == 0) {
						spdlog::info("启用 extension: {}", extension.extensionName);
						extensionsName.push_back(extension.extensionName);
					}
				}
			}
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsName.size());
			createInfo.ppEnabledExtensionNames = extensionsName.data();

			for (const auto& i : extensionsName) {
				spdlog::info("启用 extension: '{}'", i);
			}
		}

		auto res = check_validation_layers();

		if (res) {
			auto& layers = res.value();
			for (const auto layer : layers) {
				for (const auto enableLayer : enableLayers) {
					if (strcmp(layer.layerName, enableLayer) == 0) {
						spdlog::info("{}:{}", strlen(enableLayer), enableLayer);
						layersName.push_back(enableLayer);
					}
				}
			}

			createInfo.enabledLayerCount = static_cast<uint32_t>(layersName.size());
			createInfo.ppEnabledLayerNames = layersName.data();

			for (const auto& i : layersName) {
				spdlog::info("启用 layer: '{}'", i);
			}
		}

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) { // 创建调试消息回调
			fill_vulkan_debug_message_create_info(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		} else {
			createInfo.pNext = nullptr;
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &vkInstance) == VK_SUCCESS) {
			spdlog::info("创建 Vulkan 实例成功");
		} else {
			spdlog::info("创建 Vulkan 实例失败");
			return std::nullopt;
		}
		return true;
	}

	optional<bool> Render::init_vulkan_debug_message() {
		if (!enableValidationLayers) return std::nullopt;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		fill_vulkan_debug_message_create_info(createInfo);

		if (create_debug_utils_message_ext(vkInstance, &createInfo, nullptr, &vkDebugMessenger) != VK_SUCCESS) {
			spdlog::error("创建 vulkan debug message 失败");
			return std::nullopt;
		}
		return true;
	}

	optional<bool> Render::select_physical_device() {
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(vkInstance, &count, nullptr);
		if (count == 0) {
			spdlog::error("vkEnumeratePhysicalDevices : 没有找到可用的支持vulkan的物理设备");
			return std::nullopt;
		}

		vector<VkPhysicalDevice> device(count);
		vkEnumeratePhysicalDevices(vkInstance, &count, device.data());

		vector<tuple<VkPhysicalDevice, int>> deviceScore{};

		// 设备支持性评分
		auto lambda_rateDevice = [&](const VkPhysicalDevice& device) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			spdlog::info("检测设备功能支持:Wide Lines {}", features.wideLines ? "[YES]" : "[NO]");
			spdlog::info("检测设备功能支持:Large Points {}", features.largePoints ? "[YES]" : "[NO]");
			spdlog::info("检测设备功能支持:ASTC {}", features.textureCompressionASTC_LDR ? "[YES]" : "[NO]");
			spdlog::info("检测设备功能支持:BC {}", features.textureCompressionBC ? "[YES]" : "[NO]");

			int score = 0;

			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				score += 1000;
			}

			score += properties.limits.maxImageDimension2D;

			if (!features.geometryShader) {
				return 0;
			}

			return score;
			};

		for (const auto& i : device) {
			int score = lambda_rateDevice(i);
			deviceScore.push_back(make_tuple(i, score));
		}

		std::sort(deviceScore.begin(), deviceScore.end(), [](const tuple<VkPhysicalDevice, int>& a, const tuple<VkPhysicalDevice, int>& b) {
			return get<1>(a) > get<1>(b);
			});

		for (const auto& i : deviceScore) {
			spdlog::info("物理设备评分: {}", get<1>(i));
		}

		auto lambda_check_device_support_extensions = [&](const VkPhysicalDevice& device) -> bool {
			uint32_t count = 0;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);

			vector<VkExtensionProperties> extensions(count);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());

			flat_hash_set<string> requiredExtensions(enableDeviceExtensions.begin(), enableDeviceExtensions.end());
			for (const auto& i : extensions) {
				if (requiredExtensions.erase(i.extensionName) == 1) {
					spdlog::info("检查设备扩展支持: {} [YES]", i.extensionName);
				}
			}

			for (const auto& i : requiredExtensions) {
				spdlog::error("检查设备扩展支持: {} [NO]", i);
			}

			return requiredExtensions.empty();
			};

		// 检查设备是否合适
		auto lambda_IsDeviceSuitable = [&](const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			QueueFamilyIndices      indices = find_queue_families(device);

			bool                    enstensionSupport = lambda_check_device_support_extensions(device);

			SwapChainSupportDetails swapChainSupportDetails = get_swap_chain_detail(device, surface);

			bool                    swapChainCanUse = !swapChainSupportDetails.formats.empty() && !swapChainSupportDetails.presentModes.empty();

			if (!swapChainCanUse) {
				spdlog::error("检查设备交换链支持: [NO]");
			} else {
				spdlog::info("检查设备交换链支持: [YES]");
			}

			return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices.isComplete() && enstensionSupport && swapChainCanUse &&
				features.geometryShader && features.wideLines && features.largePoints;
			};

		int idx = 0;
		for (const auto& i : device) {
			if (lambda_IsDeviceSuitable(i, vkSurface)) {
				vkPhysicalDevice = i;
				break;
			}
			idx++;
		}

		if (vkPhysicalDevice == VK_NULL_HANDLE) {
			spdlog::error("没有找到合适的物理设备");
			return std::nullopt;
		}
		return true;
	}

	// 查看交换链支持情况
	Render::SwapChainSupportDetails Render::get_swap_chain_detail(VkPhysicalDevice device, const VkSurfaceKHR& surface) {
		SwapChainSupportDetails details{};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
		details.formats.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, details.formats.data());

		count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
		details.presentModes.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, details.presentModes.data());

		return details;
	}
	VkExtent2D Render::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
		capabilities.currentExtent;

		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	};

	optional<bool> Render::create_logical_device_and_queue() {
		if (vkPhysicalDevice == VK_NULL_HANDLE) return std::nullopt;

		QueueFamilyIndices              indices = find_queue_families(vkPhysicalDevice);

		vector<VkDeviceQueueCreateInfo> queueCreafeInfo{};
		flat_hash_set<uint32_t>         uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float                           queuePriority = 1.0f; // 队列的优先级

		for (auto& i : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreafeInfo.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.wideLines = true;
		deviceFeatures.largePoints = true;
		deviceFeatures.samplerAnisotropy = true;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = queueCreafeInfo.size();
		deviceCreateInfo.pQueueCreateInfos = queueCreafeInfo.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = enableDeviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = enableDeviceExtensions.data();

		if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) == VK_SUCCESS) {
			spdlog::info("创建逻辑设备成功");
		} else {
			spdlog::error("创建逻辑设备失败");
			return std::nullopt;
		}

		vkGetDeviceQueue(vkDevice, indices.graphicsFamily.value(), 0, &vkGraphicsQueue);
		if (vkGraphicsQueue == VK_NULL_HANDLE) {
			spdlog::error("获取Graphics队列失败");
			return std::nullopt;
		} else {
			spdlog::info("获取Graphics队列成功");
		}

		vkGetDeviceQueue(vkDevice, indices.presentFamily.value(), 0, &vkPresentQueue);
		if (vkGraphicsQueue == VK_NULL_HANDLE) {
			spdlog::error("获取Present队列失败");
			return std::nullopt;
		} else {
			spdlog::info("获取Present队列成功");
		}

		return true;
	}

	optional<bool> Render::create_swap_chain() {
		SwapChainSupportDetails details = get_swap_chain_detail(vkPhysicalDevice, vkSurface);
		QueueFamilyIndices      indices = find_queue_families(vkPhysicalDevice);
		// 查找理想格式
		VkSurfaceFormatKHR formatRes = [&]() -> VkSurfaceFormatKHR {
			for (const auto& i : details.formats) {
				if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					spdlog::info("查找到格式：VK_FORMAT_B8G8R8A8_SRGB   VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
					return i;
				}
			}
			return details.formats[0];
			}();

			VkPresentModeKHR presentRes = [&]() -> VkPresentModeKHR {
				for (const auto& i : details.presentModes) {
					if (i == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR) {
						spdlog::info("查找到格式：VK_PRESENT_MODE_MAILBOX_KHR");
						return i;
					}
				}
				return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
				}();

				VkExtent2D extentRes = choose_swap_extent(details.capabilities);
				swapChainCurrentExtent = extentRes;
				uint32_t imageCount = details.capabilities.minImageCount + 1;
				if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
					imageCount = details.capabilities.maxImageCount;
				}

				VkSwapchainCreateInfoKHR createInfo{};
				createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				createInfo.surface = vkSurface;
				createInfo.surface = vkSurface;
				createInfo.minImageCount = imageCount;
				createInfo.imageFormat = formatRes.format;
				createInfo.imageColorSpace = formatRes.colorSpace;
				createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				createInfo.imageExtent = extentRes;
				createInfo.imageArrayLayers = 1;

				vkSwapChainImageFormat = formatRes.format;

				uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
				if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
					createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					createInfo.queueFamilyIndexCount = 2;
					createInfo.pQueueFamilyIndices = queueFamilyIndices;
				} else {
					createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					createInfo.queueFamilyIndexCount = 0;
					createInfo.pQueueFamilyIndices = nullptr;
				}
				createInfo.preTransform = details.capabilities.currentTransform;
				createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

				if (vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &vkSwapChain) == VK_SUCCESS) {
					spdlog::info("创建交换链成功");
				} else {
					spdlog::error("创建交换链失败");
					return std::nullopt;
				}

				uint32_t swapchainImageCount = 0;
				vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapchainImageCount, nullptr);
				vkSwapChainImages.resize(swapchainImageCount);
				vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapchainImageCount, vkSwapChainImages.data());
				spdlog::info("交换链图片数量：{}", swapchainImageCount);

				return true;
	}

	optional<bool> Render::create_swap_chain_image_view() {
		vkSwapChainImageViews.resize(vkSwapChainImages.size());

		for (size_t i = 0; i < vkSwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = vkSwapChainImages[i];
			createInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = vkSwapChainImageFormat;
			createInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.subresourceRange.levelCount = 1;

			if (vkCreateImageView(vkDevice, &createInfo, nullptr, &vkSwapChainImageViews[i]) == VK_SUCCESS) {
				spdlog::info("创建Swap Chain Image View {} 成功", i);
			} else {
				spdlog::error("创建Swap Chain Image View {} 失败", i);
				return std::nullopt;
			}
		}

		return true;
	}

	optional<bool> Render::create_render_pass() {

		// 创建RenderPass
		// 先创建一个附件描述
		VkAttachmentDescription colorDesc{};
		colorDesc.format = vkSwapChainImageFormat;
		colorDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;      // 加载时候清除，也可以选择保留，也可以选择不关心
		colorDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;     // 存储阶段保存， 当然也可以选择dont care 内容将不定义
		colorDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // 不关心模板缓冲区
		colorDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 不关心模板缓冲区

		// 相当于DX中的状态，比如变成复制对象，变成复制源，作为rendertarget使用等等..
		colorDesc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		colorDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // 最终状态，这里是呈现到屏幕上

		VkAttachmentDescription deptherDesc{};
		deptherDesc.format = VK_FORMAT_D32_SFLOAT;
		deptherDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		deptherDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		deptherDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		deptherDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		deptherDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		deptherDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		deptherDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// initialLayout 指定在渲染过程开始之前图像将具有的布局。
		// finalLayout 指定渲染过程完成时自动转换到的布局。使用 VK_IMAGE_LAYOUT_UNDEFINED 代替 initialLayout
		// 意味着我们不关心图像之前的布局。这个特殊值的警告是，图像的内容不能保证被保留，但这并不重要，因为我们无论如何都会清除它。
		// 我们希望图像在渲染后可以使用交换链进行演示，这就是为什么我们使用VK_IMAGE_LAYOUT_PRESENT_SRC_KHR 作为 finalLayout 。

		VkAttachmentReference colorAttachmentRef{}; // 这个是引用，引用上面的附件描述
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentRef.attachment = 0;          // 因为到时候有个数组，这里是第一个

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentRef.attachment = 1;

		VkSubpassDescription subpassDesc{};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &colorAttachmentRef;
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

		VkAttachmentDescription tmp[2]{ colorDesc, deptherDesc };
		VkRenderPassCreateInfo  renderPassCreateInfo{};
		renderPassCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 2;
		renderPassCreateInfo.pAttachments = tmp;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDesc;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency; 

		if (vkCreateRenderPass(vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
			spdlog::error("创建RenderPass失败");
			return std::nullopt;
		} else {
			spdlog::info("创建RenderPass成功");
		}
		return true;
	}

	optional<bool> Render::create_uniform_buffer() {

		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		int          countBuffers = MAX_FRAMES_IN_FLIGHTS;
		uniformBuffer.resize(countBuffers);
		uniformMem.resize(countBuffers);
		uniformMap.resize(countBuffers);

		for (size_t i = 0; i < countBuffers; i++) {
			/* code */
			std::tie(uniformBuffer[i], uniformMem[i]) = create_buffer(
				bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			if (uniformBuffer[i] == VK_NULL_HANDLE || uniformMem[i] == VK_NULL_HANDLE) {
				spdlog::error("创建UniformBuffer {} 失败", i);
				return std::nullopt;
			}
			vkMapMemory(vkDevice, uniformMem[i], 0, bufferSize, 0, &uniformMap[i]);
		}

		return true;
	}

	optional<bool> Render::create_descriptor_set_layout() {

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding    binds[] = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = sizeof(binds) / sizeof(VkDescriptorSetLayoutBinding);
		layoutInfo.pBindings = binds;

		if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &vkDescLayout) != VK_SUCCESS) {
			spdlog::error("创建DescriptorSetLayout失败");
			return std::nullopt;
		} else {
			spdlog::info("创建DescriptorSetLayout成功");
		}

		return true;
	}

	optional<bool> Render::create_descriptor_pool() {

		VkDescriptorPoolSize size{};
		size.descriptorCount = MAX_FRAMES_IN_FLIGHTS;
		size.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		VkDescriptorPoolSize size2{};
		size2.descriptorCount = MAX_FRAMES_IN_FLIGHTS;
		size2.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkDescriptorPoolSize       sizes[] = { size, size2 };

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = MAX_FRAMES_IN_FLIGHTS;
		poolInfo.poolSizeCount = sizeof(sizes) / sizeof(VkDescriptorPoolSize);
		poolInfo.pPoolSizes = sizes;

		if (vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &vkDescPool) != VK_SUCCESS) {
			spdlog::error("创建DescriptorPool失败");
			return std::nullopt;
		} else {
			spdlog::info("创建DescriptorPool成功");
		}

		return true;
	}

	optional<bool> Render::create_descriptor_sets() {

		vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHTS, vkDescLayout);

		VkDescriptorSetAllocateInfo   allocInfo{};
		allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = vkDescPool;
		allocInfo.descriptorSetCount = 2;
		allocInfo.pSetLayouts = layouts.data();

		vkDescSets.resize(MAX_FRAMES_IN_FLIGHTS);
		if (vkAllocateDescriptorSets(vkDevice, &allocInfo, vkDescSets.data()) != VK_SUCCESS) {
			spdlog::error("创建DescriptorSets失败");
			return std::nullopt;
		} else {
			spdlog::info("创建DescriptorSets成功");
		}
		spdlog::info("DescriptorSets数量: {}", vkDescSets.size());

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHTS; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = testImageView;
			imageInfo.sampler = testSampler;

			VkWriteDescriptorSet descriptorWrite[2]{};
			descriptorWrite[0].sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite[0].dstSet = vkDescSets[i];
			descriptorWrite[0].dstArrayElement = 0;
			descriptorWrite[0].dstBinding = 0;
			descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite[0].descriptorCount = 1;
			descriptorWrite[0].pBufferInfo = &bufferInfo;

			descriptorWrite[1].sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite[1].dstSet = vkDescSets[i];
			descriptorWrite[1].dstArrayElement = 0;
			descriptorWrite[1].dstBinding = 1;
			descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite[1].descriptorCount = 1;
			descriptorWrite[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(vkDevice, sizeof(descriptorWrite) / sizeof(VkWriteDescriptorSet), descriptorWrite, 0, nullptr);
		}

		return true;
	}

	optional<bool> Render::create_graphics_pipeline() {

		string vertStr = "                  \
            #version 450                                          \n\
            layout(binding = 0) uniform UniformBufferObject {     \n\
            mat4 model;                                           \n\
            mat4 view;                                            \n\
            mat4 proj;                                            \n\
            } ubo;                                                \n\
            layout(location = 0) in vec3 vpos;                    \n\
            layout(location = 1) in vec3 vcolors;                 \n\
            layout(location = 2) in vec2 vuv;                     \n\
                                                                  \n\
            layout(location = 0) out vec3 fragcolor;              \n\
            layout(location = 1) out vec2 fraguv;		  \n\
                                                                  \n\
            void main() {                                         \n\
                  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vpos, 1.0);             \n\
                  gl_PointSize = 4.0f;                            \n\
                  fragcolor = vcolors;                            \n\
                  fraguv = vuv;					  \n\
            }                                                     \n\
      ";

		string fragStr = "            #version 450                                    \n\
                                    layout(location = 0) in vec3 fragcolor;         \n\
                                    layout(location = 1) in vec2 fraguv;         \n\
                                    layout(location = 0) out vec4 outColor;         \n\
                                    layout(binding = 1) uniform sampler2D samp;	    \n\
                                    void main() {                                   \n\
                                          outColor = texture(samp, fraguv); \n\
                                         //outColor = vec4(fraguv.x, fraguv.y, 1.0, 1.0);                     \n\
                                    }                                               \n\
      ";

		auto   vSpriv = compile_glsl_spriv("vertshader", vertStr, GLSLANG_STAGE_VERTEX);
		if (!vSpriv) {
			spdlog::error("vert着色器编译失败");
			return std::nullopt;
		}

		auto fSpriv = compile_glsl_spriv("fragshader", fragStr, GLSLANG_STAGE_FRAGMENT);
		if (!fSpriv) {
			spdlog::error("frag着色器编译失败");
			return std::nullopt;
		}

		spdlog::info("vert和farg着色器编译成功");

		auto resVert = create_shader_module("vert", std::move(vSpriv.value()));
		auto resFrag = create_shader_module("frag", std::move(fSpriv.value()));

		if (!(resVert && resFrag)) {
			return std::nullopt;
		}

		spdlog::info("着色器模块全部创建成功");

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.module = resVert.value();
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.module = resFrag.value();
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineShaderStageCreateInfo  shaderStage[] = { vertShaderStageInfo, fragShaderStageInfo };

		vector<VkDynamicState>           dystate{ VkDynamicState::VK_DYNAMIC_STATE_SCISSOR,
						VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
						VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH };

		VkPipelineDynamicStateCreateInfo dyCreateInfo{};
		dyCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dyCreateInfo.dynamicStateCount = dystate.size();
		dyCreateInfo.pDynamicStates = dystate.data();

		// 创建顶点stage， 一般来说是放入顶点缓冲区，或者说是指定instance
		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};

		auto                                 vAttr = Vertex2::get_vertex_input_attr_desc();
		auto                                 vBind = Vertex2::get_vertex_bind_desc();

		vertexInputCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.vertexAttributeDescriptionCount = vAttr.size();
		vertexInputCreateInfo.pVertexAttributeDescriptions = vAttr.data();
		vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputCreateInfo.pVertexBindingDescriptions = &vBind;

		// 顶点装订， 指定定点是什么拓扑结构的
		VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo{};
		assemblyCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyCreateInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyCreateInfo.primitiveRestartEnable = false;

		// 创建视口大小
		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = windowWidth;
		viewport.height = windowHeight;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;

		// 创建裁剪区域 (当前是不裁剪任何区域，全屏都显示)
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainCurrentExtent;

		// 把视口信息和裁剪区域信息放入视口创建信息中
		VkPipelineViewportStateCreateInfo viewportCreateInfo{};
		viewportCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportCreateInfo.pScissors = &scissor;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pViewports = &viewport;
		viewportCreateInfo.viewportCount = 1;

		// 创建了着色器模块并且都放入了着色器阶段信息中
		// 指明了一些stage是动态的(DynamicStage),
		// 指明了顶点Stage输入的信息(这边我们不引入顶点缓冲区， 也没有引入instance)
		// 指明了顶点装订拓扑信息,
		// 设置了视口信息(viewport, 裁剪区域)

		// 现在设置一下光栅化器的配置
		VkPipelineRasterizationStateCreateInfo rasterCreateInfo{};

		rasterCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterCreateInfo.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE; // 这里丢弃背面
		rasterCreateInfo.depthClampEnable = false;                                 // 目前设置了超出最大深度的部分会被丢弃
		rasterCreateInfo.rasterizerDiscardEnable = false; // 如果这个设置成true，那么没有几何会通过光栅化， 相当于什么都不画
		rasterCreateInfo.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL; // 这里设置了填充模式， 也就是说填充三角形的内部, 可以画点, 画线
		// rasterCreateInfo.lineWidth = 4.0;                                   // 任何比1.0粗的线都要开启wideLines功能
		rasterCreateInfo.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE; // 这里设置了顺时针为正面

		// 光栅化器可以通过添加常数值或基于片段的斜率偏置深度值来改变深度值。这有时用于阴影贴图，但我们不会使用它。将 depthBiasEnable 设置为 false
		rasterCreateInfo.depthBiasEnable = false;
		rasterCreateInfo.depthBiasConstantFactor = 0.0; // Optional
		rasterCreateInfo.depthBiasClamp = 0.0; // Optional
		rasterCreateInfo.depthBiasSlopeFactor = 0.0; // Optional

		// 多重采样
		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
		multisampleCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleCreateInfo.sampleShadingEnable = false;
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCreateInfo.minSampleShading = 1.0;
		multisampleCreateInfo.pSampleMask = nullptr;
		multisampleCreateInfo.alphaToCoverageEnable = false;
		multisampleCreateInfo.alphaToOneEnable = false;

		// 颜色混合方案
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

		// 颜色阶段设置
		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
		colorBlendCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendCreateInfo.attachmentCount = 1;
		colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
		colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendCreateInfo.logicOpEnable = false;
		colorBlendCreateInfo.blendConstants[0] = 0.0f;
		colorBlendCreateInfo.blendConstants[1] = 0.0f;
		colorBlendCreateInfo.blendConstants[2] = 0.0f;
		colorBlendCreateInfo.blendConstants[3] = 0.0f;

		// 创建管线布局, 着色器引用的uniform值和推送值，可在绘制时更新
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &vkDescLayout;

		if (vkCreatePipelineLayout(vkDevice, &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS) {
			spdlog::error("创建管线布局失败");
			return std::nullopt;
		} else {
			spdlog::info("创建管线布局成功");
		}

		// 创建Depth info
		VkPipelineDepthStencilStateCreateInfo dinfo{};
		dinfo.depthWriteEnable = true;
		dinfo.depthTestEnable = true;
		dinfo.depthBoundsTestEnable = false;
		dinfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		dinfo.stencilTestEnable = false;
		dinfo.depthCompareOp = VK_COMPARE_OP_LESS;

		// 创建图形流水线
		// 回顾过程 , 我们需要为创建图形流水线提供这些配置:
		// 着色器stage
		// 可修改的阶段属性,比如视口大小,线宽, 点的大小
		// 顶点输入源
		// 顶点装配布局
		// 视口和裁剪
		// 光栅化
		// 多重采样
		// 颜色Blend阶段
		// stencil和depth测试我们暂时不关心， 设置为Nullptr

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStage;
		pipelineCreateInfo.pDynamicState = &dyCreateInfo;
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &assemblyCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &dinfo;
		// 然后引用pipelineLayout
		pipelineCreateInfo.layout = vkPipelineLayout;
		// 加入renderpass
		pipelineCreateInfo.renderPass = vkRenderPass;
		pipelineCreateInfo.subpass = 0; // 子通道的索引， 这里相当于使用第一个自通道，不是数量
		pipelineCreateInfo.basePipelineHandle = nullptr;

		// 这东西允许pipeline继承，听说继承创建新的graphic pipeline成本很低, 不过目前我们不使用，所以设置为-1
		// 如果要使用继承，只有在 VkGraphicsPipelineCreateInfo 的 flags 字段中还指定了 VK_PIPELINE_CREATE_DERIVATIVE_BIT 标志时才使用这些值。
		pipelineCreateInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(vkDevice, nullptr, 1, &pipelineCreateInfo, nullptr, &vkGraphicsPipeline) != VK_SUCCESS) {
			spdlog::error("创建图形流水线失败");
			return std::nullopt;
		} else {
			spdlog::info("创建图形流水线成功");
		}

		// 使用完毕之后删除着色器模块
		vkDestroyShaderModule(vkDevice, resVert.value(), nullptr);
		vkDestroyShaderModule(vkDevice, resFrag.value(), nullptr);

		return true;
	}

	optional<bool> Render::create_framebuffers() {
		vkSwapChainFramebuffers.resize(vkSwapChainImageViews.size());

		// 我们首先需要指定帧缓冲区需要与哪个 renderPass 兼容。
		// 只能将帧缓冲区与其兼容的渲染过程一起使用，这大致意味着它们使用相同数量和类型的附件。

		for (int i = 0; i < vkSwapChainFramebuffers.size(); i++) {
			VkImageView             dp[2] = { vkSwapChainImageViews[i], depthImageView };
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.attachmentCount = 2;
			framebufferCreateInfo.pAttachments = dp;
			framebufferCreateInfo.height = swapChainCurrentExtent.height;
			framebufferCreateInfo.width = swapChainCurrentExtent.width;
			framebufferCreateInfo.layers = 1;
			framebufferCreateInfo.renderPass = vkRenderPass;

			if (vkCreateFramebuffer(vkDevice, &framebufferCreateInfo, nullptr, &vkSwapChainFramebuffers[i]) != VK_SUCCESS) {
				spdlog::error("创建帧缓冲区失败");
				return std::nullopt;
			} else {
				spdlog::info("创建帧缓冲区{}成功", i);
			}
		}
		spdlog::info("创建帧缓冲区成功");

		return true;
	}

	optional<bool> Render::create_command_pool() {
		QueueFamilyIndices      indices = find_queue_families(vkPhysicalDevice);

		VkCommandPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

		if (vkCreateCommandPool(vkDevice, &poolCreateInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
			spdlog::error("创建命令池失败");
			return std::nullopt;
		} else {
			spdlog::info("创建命令池成功");
		}

		return true;
	}

	optional<bool> Render::create_test_model() {

		using namespace Clipper2Lib;

		// PathsD rec1{Clipper2Lib::RectD(-0.75, -0.75, 0.75, 0.75).AsPath()};
		// PathsD rec2{Clipper2Lib::RectD(0.25, 0.25, 0.5, 0.5).AsPath()};
		// PathsD res{};
		// res = Difference(rec1, rec2, FillRule::NonZero);

		// PathsD subject, clip, solution;
		// subject.push_back(MakePathD({100, 50, 10, 79, 65, 2, 65, 98, 10, 21}));
		// clip.push_back(MakePathD({98, 63, 4, 68, 77, 8, 52, 100, 19, 12}));
		// solution = Union(subject, clip, FillRule::NonZero);

		// PathsD res{};
		// res.push_back(Ellipse(PointD{100, 100}, 30, 30));

		PathD  pattern = Ellipse(PointD(100, 100), 15, 15);
		PathD  path = MakePathD({ 0, 0, 100, 0, 100, 100, 0, 100, 0, 0 });
		PathsD solution = MinkowskiSum(pattern, path, true);
		Rect   rc = GetBounds(solution);

		auto   fit = [](double a, double b, double x) {
			double tempW = b - a;
			double tempX = x - a;
			return (x - a) / (b - a);
			};
		auto calcUv = [&](double x, double y) -> glm::dvec2 {
			return {
			      fit(rc.left, rc.right, x),
			      fit(rc.top, rc.bottom, y),
			};
			};

		// res.push_back({});

		// indices = Warp::earcut<uint32_t>(solution);
		// vertices.clear();
		// for (auto& i : solution) {
		//       for (auto& j : i) {
		//             spdlog::info("x:{}, y:{}", j.x * 0.005 - 0.5, j.y * 0.005 - 0.5);
		//             vertices.push_back({
		//                 { j.x * 0.005 - 0.5, j.y * 0.005 - 0.5, 0.0f },
		//                 { 1.0f, 0.0f, 0.0f },
		//                 calcUv(j.x, j.y)
		//             });
		//       }
		// }

		tinyobj::attrib_t                attrib;
		std::vector<tinyobj::shape_t>    shapes{};
		std::vector<tinyobj::material_t> mats{};
		std::string                      err;
		tinyobj::LoadObj(&attrib, &shapes, &mats, &err, "../../../Resource/A.obj");
		vertices = {};
		indices = {};
		vertices.reserve(4096);
		indices.reserve(4096);

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex2 vertex{};

				vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
						    attrib.vertices[3 * index.vertex_index + 1],
						    attrib.vertices[3 * index.vertex_index + 2] };

				vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0], 1.0 - attrib.texcoords[2 * index.texcoord_index + 1] };

				vertex.color = { 1.0f, 1.0f, 1.0f };

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}
		}

		/*vertices = {
		      {{ -0.5f, -0.5f, 0.0f },   { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
		      { { 0.5f, -0.5f, 0.0f },   { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
		      { { 0.5f, 0.5f, 0.0f },    { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
		      { { -0.5f, 0.5f, 0.0f },   { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }},

		      { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
		      { { 0.5f, -0.5f, -0.5f },  { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
		      { { 0.5f, 0.5f, -0.5f },   { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
		      { { -0.5f, 0.5f, -0.5f },  { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }}
		};

		indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };*/
		return true;
	}

	optional<bool> Render::create_vertex_buffer() {

		VkDeviceSize vsize = sizeof(vertices[0]) * vertices.size();
		auto [local, localm] =
			create_buffer(vsize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (local == VK_NULL_HANDLE || localm == VK_NULL_HANDLE) {
			spdlog::error("创建顶点缓冲区失败");
			return std::nullopt;
		}
		void* data;
		vkMapMemory(vkDevice, localm, 0, vsize, 0, &data);
		memcpy(data, vertices.data(), vsize);
		vkUnmapMemory(vkDevice, localm);

		std::tie(vkVertexBuffer, vkVertexBufferMemory) =
			create_buffer(vsize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (vkVertexBuffer == VK_NULL_HANDLE || vkVertexBufferMemory == VK_NULL_HANDLE) {
			spdlog::error("创建顶点缓冲区失败");
			return std::nullopt;
		}
		copy_buffer(local, vkVertexBuffer, vsize);
		vkDestroyBuffer(vkDevice, local, nullptr);
		vkFreeMemory(vkDevice, localm, nullptr);

		return true;
	}

	optional<bool> Render::create_index_buffer() {
		VkDeviceSize vsize = sizeof(indices[0]) * indices.size();

		auto [local, localm] =
			create_buffer(vsize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!local || !localm) {
			spdlog::error("创建索引缓冲区失败");
			return std::nullopt;
		}

		void* data;
		vkMapMemory(vkDevice, localm, 0, vsize, 0, &data);
		memcpy(data, indices.data(), vsize);
		vkUnmapMemory(vkDevice, localm);

		std::tie(vkIndexBuffer, vkIndexBufferMemory) =
			create_buffer(vsize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (!vkIndexBuffer || !vkIndexBufferMemory) {
			spdlog::error("创建索引缓冲区失败");
			return std::nullopt;
		}

		copy_buffer(local, vkIndexBuffer, vsize);
		vkDestroyBuffer(vkDevice, local, nullptr);
		vkFreeMemory(vkDevice, localm, nullptr);

		return true;
	}

	optional<bool> Render::create_command_buffer() {

		vkCommandBuffer.resize(MAX_FRAMES_IN_FLIGHTS);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHTS;
		commandBufferAllocateInfo.commandPool = vkCommandPool;

		if (vkAllocateCommandBuffers(vkDevice, &commandBufferAllocateInfo, vkCommandBuffer.data()) != VK_SUCCESS) {
			spdlog::error("创建命令缓冲区失败");
			return std::nullopt;
		} else {
			spdlog::info("创建命令缓冲区成功");
		}

		return true;
	}

	optional<bool> Render::record_command(VkCommandBuffer cmdBuffer, uint32_t imageIndex) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		beginInfo.flags = 0;

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = vkRenderPass;
		renderPassBeginInfo.framebuffer = vkSwapChainFramebuffers[imageIndex];

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapChainCurrentExtent;

		VkClearValue clearColor[2] = {};

		clearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearColor[1].depthStencil = { 1.0f, 0 };
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearColor;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		vkCmdBindPipeline(cmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);
		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.height = swapChainCurrentExtent.height;
		viewport.width = swapChainCurrentExtent.width;
		viewport.x = 0;
		viewport.y = 0;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.extent = swapChainCurrentExtent;
		scissor.offset = { 0, 0 };

		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(cmdBuffer, 4.0f);

		VkBuffer     bff[] = { vkVertexBuffer };
		VkDeviceSize offests[] = { 0 };

		vkCmdBindDescriptorSets(
			cmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescSets[currentFrame], 0, nullptr
		);
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, bff, offests);

		vkCmdBindIndexBuffer(cmdBuffer, vkIndexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmdBuffer, indices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(cmdBuffer);

		if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
			spdlog::error("结束渲染通道失败");
			return std::nullopt;
		}

		return true;
	}

	optional<bool> Render::create_semaphores() {

		inFlightFences.reserve(MAX_FRAMES_IN_FLIGHTS);
		vkImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHTS);
		vkRenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHTS);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; i++) {
			if (vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkImageAvailableSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkRenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				spdlog::error("创建图像信号量和Fence创建失败");
				return std::nullopt;
			}
		}

		return true;
	}

	void Render::update_uniform_buffer(int32_t imageIndex) {
		static float        p = 0.0;
		static double       lastTime = glfwGetTime();

		double now = glfwGetTime();
		double              currTime = now - lastTime;
		lastTime = now;
		//spdlog::info("{}", currTime);
		UniformBufferObject ubo{};

		if (map.GetBool(BT_D)) {
			p += currTime * glm::radians(30.0);

		} else if (map.GetBool(BT_A)) {
			p += currTime * glm::radians(-30.0);
		}

		// ubo.model = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1));
		// ubo.view  = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		// ubo.proj  = glm::perspective(75.0f, swapChainCurrentExtent.width / (float)swapChainCurrentExtent.height, 0.1f, 1000.0f);
		// ubo.proj[1][1] *= -1;

		ubo.model = glm::rotate(glm::mat4(1.0f), p, glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainCurrentExtent.width / (float)swapChainCurrentExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformMap[imageIndex], &ubo, sizeof(ubo));
	}

	/**
	 * @brief
	 * @param  size
	 * @param  usage
	 * @param  properties
	 * @return
	 */
	tuple<VkBuffer, VkDeviceMemory> Render::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {

		VkBuffer           buffer;
		VkDeviceMemory     bufferMem;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vkDevice, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
			spdlog::error("buffer失败");
			return tuple(VK_NULL_HANDLE, VK_NULL_HANDLE);
		}

		// 从顶点buffer获取内存需求
		VkMemoryRequirements memoryRequirements{};
		vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryRequirements);

		// 为顶点buffer分配实际内存
		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		// 获取内存索引，这个索引需要我们询问vulkan: 如果我们要VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT和VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  (COHERENT连贯)
		// 类型的内存，索引应该是什么
		memoryAllocateInfo.memoryTypeIndex = find_memory_type(memoryRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vkDevice, &memoryAllocateInfo, nullptr, &bufferMem) != VK_SUCCESS) {
			spdlog::error("buffer内存分配失败");
			return tuple(VK_NULL_HANDLE, VK_NULL_HANDLE);
		}
		vkBindBufferMemory(vkDevice, buffer, bufferMem, 0);

		return { buffer, bufferMem };
	}

	void Render::create_test_image() {
		int      x, y, c;
		uint8_t* data = stbi_load("../../../Resource/A.png", &x, &y, &c, STBI_rgb_alpha);
		int      size = x * y * 4;

		auto [b, m] = create_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		spdlog::info("{} {} {}", x, y, c);
		auto res = create_image(
			vkDevice,
			{ x, y },
			VkFormat::VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		if (res) {
			std::tie(testImage, testImageMem) = res.value();
			spdlog::info("创建图片GPU缓冲区成功");

			void* pa;
			vkMapMemory(vkDevice, m, 0, size, 0, &pa);
			memcpy(pa, data, size);
			vkUnmapMemory(vkDevice, m);
			stbi_image_free(data);
			translation_image_layout(testImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			copy_buffer_2_image(b, testImage, { x, y });
			translation_image_layout(
				testImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);

			// start_single_time_commands([&](VkCommandBuffer buf) {
			//       // vkCmdCopyBufferToImage(buf, b, testImage, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
			// });

		} else {
			spdlog::error("创建图片GPU缓冲区失败");
			return;
		}

		vkFreeMemory(vkDevice, m, nullptr);
		vkDestroyBuffer(vkDevice, b, nullptr);

		auto viewRes = create_image_view(testImage, VK_FORMAT_R8G8B8A8_SRGB);
		if (viewRes) {
			spdlog::info("Create Image View");
			testImageView = viewRes.value();
		} else {
			spdlog::info("Create Image View Failed");
			return;
		}
		VkPhysicalDeviceProperties ps{};
		vkGetPhysicalDeviceProperties(vkPhysicalDevice, &ps);

		VkSamplerCreateInfo sinfo{};
		sinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sinfo.anisotropyEnable = true;
		sinfo.maxAnisotropy = ps.limits.maxSamplerAnisotropy;
		sinfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		sinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sinfo.magFilter = VK_FILTER_LINEAR;
		sinfo.minFilter = VK_FILTER_LINEAR;
		sinfo.unnormalizedCoordinates = VK_FALSE;
		sinfo.compareEnable = VK_TRUE;
		sinfo.compareOp = VK_COMPARE_OP_ALWAYS;
		sinfo.minLod = 0.0f;
		sinfo.maxLod = 0.0f;
		sinfo.mipLodBias = 0.0f;
		sinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(vkDevice, &sinfo, nullptr, &testSampler) != VK_SUCCESS) {
			return;
		}

		/* VkImageViewCreateInfo vinfo{};
		 vinfo.components                      = VK_COMPONENT_SWIZZLE_R | VK_COMPONENT_SWIZZLE_G | VK_COMPONENT_SWIZZLE_B | VK_COMPONENT_SWIZZLE_A;
		 vinfo.format                          = VK_FORMAT_R8G8B8A8_SRGB;
		 vinfo.image                           = testImage;
		 vinfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		 vinfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		 vinfo.subresourceRange.layerCount     = 1;
		 vinfo.subresourceRange.baseMipLevel   = 0;
		 vinfo.subresourceRange.levelCount     = 1;
		 vinfo.subresourceRange.baseArrayLayer = 0;
		 vinfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;

		 vkCreateImageView(vkDevice, &vinfo, nullptr, &testImageView);*/
	}

	void Render::create_depth_image() {

		auto res = create_image(
			vkDevice,
			{ swapChainCurrentExtent.width, swapChainCurrentExtent.height },
			VK_FORMAT_D32_SFLOAT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		if (!res) {
			return;
		}
		std::tie(depthImage, depthImageMem) = res.value();

		auto vres = create_image_view(depthImage, VK_FORMAT_D32_SFLOAT);
		if (!vres) {
			spdlog::info("Create Depth Buffer View Failed");
			return;
		}
		depthImageView = vres.value();
		spdlog::info("Depth Image Create Success");

		translation_image_layout(depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void Render::start_single_time_commands(std::function<void(VkCommandBuffer)> func) {

		VkCommandBuffer             cmdbuffer{};
		VkCommandBufferAllocateInfo alloc{};
		alloc.commandBufferCount = 1;
		alloc.commandPool = vkCommandPool;
		alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkAllocateCommandBuffers(vkDevice, &alloc, &cmdbuffer);

		VkCommandBufferBeginInfo begininfo{};

		begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdbuffer, &begininfo);

		func(cmdbuffer);

		vkEndCommandBuffer(cmdbuffer);

		// submit

		VkSubmitInfo sminfo{};
		sminfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		sminfo.commandBufferCount = 1;
		sminfo.pCommandBuffers = &cmdbuffer;
		vkQueueSubmit(vkGraphicsQueue, 1, &sminfo, VK_NULL_HANDLE);
		vkDeviceWaitIdle(vkDevice);

		vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &cmdbuffer);
	}

	void Render::translation_image_layout(VkImage img, VkFormat format, VkImageLayout old, VkImageLayout to) {

		start_single_time_commands([&](VkCommandBuffer buf) {
			VkImageMemoryBarrier bar{};
			bar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			bar.image = img;
			bar.oldLayout = old;
			bar.newLayout = to;
			bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bar.subresourceRange.baseArrayLayer = 0;
			bar.subresourceRange.baseMipLevel = 0;
			bar.subresourceRange.layerCount = 1;
			bar.subresourceRange.levelCount = 1;
			bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			VkPipelineStageFlags src;
			VkPipelineStageFlags dst;

			if (old == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				bar.srcAccessMask = 0;
				bar.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
			} else if (old == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				bar.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				bar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				src = VK_PIPELINE_STAGE_TRANSFER_BIT;
				dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			} else if (old == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				bar.srcAccessMask = 0;
				bar.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dst = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

			} else {
				// bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			vkCmdPipelineBarrier(buf, src, dst, 0, 0, nullptr, 0, nullptr, 1, &bar);
			});
	}

	optional<VkImageView> Render::create_image_view(VkImage img, VkFormat format) {
		VkImageView           view{};

		VkImageViewCreateInfo vinfo{};
		vinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vinfo.format = format;
		vinfo.image = img;
		vinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vinfo.subresourceRange.layerCount = 1;
		vinfo.subresourceRange.baseMipLevel = 0;
		vinfo.subresourceRange.levelCount = 1;
		vinfo.subresourceRange.baseArrayLayer = 0;
		vinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		if (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
			vinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		if (vkCreateImageView(vkDevice, &vinfo, nullptr, &view) != VK_SUCCESS) {
			return std::nullopt;
		}

		return view;
	}

	void Render::copy_buffer_2_image(VkBuffer buf, VkImage img, glm::ivec2 size) {

		start_single_time_commands([&](VkCommandBuffer buffer) {
			VkBufferImageCopy cp{};
			cp.bufferImageHeight = 0;
			cp.bufferRowLength = 0;
			cp.bufferOffset = 0;

			cp.imageExtent.height = size.y;
			cp.imageExtent.width = size.x;
			cp.imageExtent.depth = 1;
			cp.imageOffset = { 0, 0, 0 };
			cp.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			cp.imageSubresource.baseArrayLayer = 0;
			cp.imageSubresource.layerCount = 1;
			cp.imageSubresource.mipLevel = 0;
			vkCmdCopyBufferToImage(buffer, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &cp);
			});
	}

	void Render::copy_image_2_buffer(VkImage img, VkBuffer buf, glm::ivec2 size) {
		start_single_time_commands([&](VkCommandBuffer cbuf) {
			VkBufferImageCopy cp{};
			cp.bufferImageHeight = 0;
			cp.bufferOffset = 0;
			cp.bufferRowLength = 0;
			cp.imageExtent.depth = 1;
			cp.imageExtent.height = size.y;
			cp.imageExtent.width = size.x;
			cp.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			cp.imageSubresource.baseArrayLayer = 0;
			cp.imageSubresource.layerCount = 1;
			cp.imageSubresource.mipLevel = 0;
			vkCmdCopyImageToBuffer(cbuf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, buf, 1, &cp);
			});
	}

	void Render::draw_window_frame() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		if (width == 0 || height == 0) {
			return;
		}

		vkWaitForFences(vkDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(vkDevice, vkSwapChain, UINT64_MAX, vkImageAvailableSemaphore[currentFrame], nullptr, &imageIndex);

		update_uniform_buffer(currentFrame);
		vkResetFences(vkDevice, 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(vkCommandBuffer[currentFrame], 0);
		record_command(vkCommandBuffer[currentFrame], imageIndex);

		// 填写提交信息
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore          seamphores[] = { vkImageAvailableSemaphore[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = seamphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCommandBuffer[currentFrame];

		VkSemaphore signalSemaphores[] = { vkRenderFinishedSemaphore[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			spdlog::error("提交命令缓冲区失败");
			return;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { vkSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;

		VkResult res = vkQueuePresentKHR(vkPresentQueue, &presentInfo);
		if (res == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized || res == VK_SUBOPTIMAL_KHR) {

			spdlog::info("调整窗口大小");
			framebufferResized = false;
			recreate_swap_chain();
		} else if (res != VK_SUCCESS) {
			spdlog::error("获取下一张图像失败");
			return;
		}
		currentFrame = (currentFrame + 1) & MAX_FRAMES_IN_FLIGHTS;
	}

	void Render::clean_swap_chain() {
		vkDestroyImageView(vkDevice, depthImageView, nullptr);
		vkDestroyImage(vkDevice, depthImage, nullptr);
		vkFreeMemory(vkDevice, depthImageMem, nullptr);

		for (size_t i = 0; i < vkSwapChainImageViews.size(); i++) {
			vkDestroyImageView(vkDevice, vkSwapChainImageViews[i], nullptr);
		}

		for (size_t i = 0; i < vkSwapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(vkDevice, vkSwapChainFramebuffers[i], nullptr);
		}

		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
	}

	void Render::recreate_swap_chain() {
		vkDeviceWaitIdle(vkDevice);
		clean_swap_chain();

		optional<bool> res = create_swap_chain()
			.and_then([&](optional<bool>) {
			create_depth_image();
			return create_swap_chain_image_view();
				})
			.and_then([&](optional<bool>) { return create_framebuffers(); });

				if (!res) {
					spdlog::error("重建交换链失败");
				}
				manager.SetDisplaySize(swapChainCurrentExtent.width, swapChainCurrentExtent.height);
	}

	uint32_t Render::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);

		// 找到合适的缓冲区类型
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				spdlog::info("找到了合适的缓冲区");

				return i;
			}
		}

		spdlog::error("没有找到合适的缓冲区");

		return 0;
	}

	void Render::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

		start_single_time_commands([&](VkCommandBuffer buf) {
			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(buf, srcBuffer, dstBuffer, 1, &copyRegion);
			});
	}

	optional<VkShaderModule> Render::create_shader_module(const string& moduleName, const vector<uint32_t>& code) {
		VkShaderModule           m{};

		VkShaderModuleCreateInfo CreateInfo{};
		CreateInfo.codeSize = code.size() * sizeof(uint32_t);
		CreateInfo.pCode = code.data();
		CreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		if (vkCreateShaderModule(vkDevice, &CreateInfo, nullptr, &m) != VK_SUCCESS) {
			spdlog::error("\"{}\"着色器模块创建失败", moduleName);
			return std::nullopt;
		}

		return m;
	}

	optional<vector<uint32_t>> Render::compile_glsl_spriv(const string& shaderName, const string& shaderSource, glslang_stage_t stage) {
		const glslang_input_t input = {
		      .language = GLSLANG_SOURCE_GLSL,
		      .stage = stage,
		      .client = GLSLANG_CLIENT_VULKAN,
		      .client_version = GLSLANG_TARGET_VULKAN_1_3,
		      .target_language = GLSLANG_TARGET_SPV,
		      .target_language_version = GLSLANG_TARGET_SPV_1_6,
		      .code = shaderSource.c_str(),
		      .default_version = 100,
		      .default_profile = GLSLANG_NO_PROFILE,
		      .force_default_version_and_profile = false,
		      .forward_compatible = false,
		      .messages = GLSLANG_MSG_DEFAULT_BIT,
		      .resource = glslang_default_resource(),
		};

		glslang_shader_t* shader = glslang_shader_create(&input);

		vector<uint32_t>  spirvBinary{};

		if (!glslang_shader_preprocess(shader, &input)) {
			spdlog::error("GLSL preprocessing failed {}\n", shaderName);
			spdlog::error("{}\n", glslang_shader_get_info_log(shader));
			spdlog::error("{}\n", glslang_shader_get_info_debug_log(shader));
			spdlog::error("{}\n", input.code);
			glslang_shader_delete(shader);
			return std::nullopt;
		}

		if (!glslang_shader_parse(shader, &input)) {

			spdlog::error("GLSL parsing failed {}\n", shaderName);
			spdlog::error("{}\n", glslang_shader_get_info_log(shader));
			spdlog::error("{}\n", glslang_shader_get_info_debug_log(shader));
			spdlog::error("{}\n", glslang_shader_get_preprocessed_code(shader));
			glslang_shader_delete(shader);
			return std::nullopt;
		}

		glslang_program_t* program = glslang_program_create();
		glslang_program_add_shader(program, shader);

		if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
			spdlog::error("GLSL linking failed {}\n", shaderName);
			spdlog::error("{}\n", glslang_program_get_info_log(program));
			spdlog::error("{}\n", glslang_program_get_info_debug_log(program));
			glslang_program_delete(program);
			glslang_shader_delete(shader);
			return std::nullopt;
		}

		glslang_program_SPIRV_generate(program, stage);

		size_t binSize = glslang_program_SPIRV_get_size(program);
		spirvBinary.resize(binSize);

		glslang_program_SPIRV_get(program, spirvBinary.data());

		const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
		if (spirv_messages) spdlog::info("({}) {}\b", shaderName, spirv_messages);

		glslang_program_delete(program);
		glslang_shader_delete(shader);

		return std::move(spirvBinary);
	}

	expected<std::tuple<VkImage, VkDeviceMemory>, WERROR> Render::create_image(
		VkDevice device, glm::ivec2 size, VkFormat format, VkImageTiling tilingType, VkImageUsageFlags usage, VkMemoryPropertyFlags properties
	) {
		VkImageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.arrayLayers = 1;
		info.format = format;
		info.tiling = tilingType;
		info.usage = usage;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		info.extent.depth = 1;
		info.extent.height = size.y;
		info.extent.width = size.x;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.pNext = nullptr;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkImage        temp;
		VkDeviceMemory mem;
		if (vkCreateImage(device, &info, nullptr, &temp) != VK_SUCCESS) {
			spdlog::error("Create VkImage Failed");
			return std::unexpected(WERROR::ERROR);
		}

		VkMemoryRequirements req{};
		vkGetImageMemoryRequirements(device, temp, &req);

		VkMemoryAllocateInfo alloc{};
		alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc.allocationSize = req.size;
		alloc.memoryTypeIndex = find_memory_type(req.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &alloc, nullptr, &mem) != VK_SUCCESS) {
			spdlog::error("分配内存失败");
			return std::unexpected(WERROR::ERROR);
		}

		vkBindImageMemory(device, temp, mem, 0);
		return make_tuple(temp, mem);
	}

	optional<bool> Render::create_surface() {
		VkResult res = glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface);

		spdlog::info("创建窗口Surface返回结果：{}", (int)res);
		if (res == VK_SUCCESS) {
			spdlog::info("创建窗口Surface成功");
		} else {
			spdlog::error("创建窗口Surface失败");
			return std::nullopt;
		}
		return true;
	}

	void Render::main_loop() {
		static double   lastTime = 1.0;
		static double   currentTime = 1.0;
		static uint32_t fps = 0;
		static double   delta = 0.0;
		MSG             msg;
		while (!glfwWindowShouldClose(window)) {

			if (PeekMessage(&msg, glfwGetWin32Window(window), 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				manager.HandleMessage(msg);
				manager.Update();
			}


			fps++;
			currentTime = glfwGetTime();
			delta = currentTime - lastTime;
			if (delta > 1.0) {
				glfwSetWindowTitle(window, fmt::format("fps:{}", fps).c_str());
				lastTime = currentTime;
				fps = 0;
			}

			glfwPollEvents();
			draw_window_frame();
		}

		vkDeviceWaitIdle(vkDevice);
	}

	void Render::end() {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; i++) {
			vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore[i], nullptr);
			vkDestroySemaphore(vkDevice, vkRenderFinishedSemaphore[i], nullptr);
			vkDestroyFence(vkDevice, inFlightFences[i], nullptr);
		}

		vkDestroyImageView(vkDevice, testImageView, nullptr);
		vkDestroyImage(vkDevice, testImage, nullptr);
		vkFreeMemory(vkDevice, testImageMem, nullptr);

		vkDestroyBuffer(vkDevice, vkIndexBuffer, nullptr);
		vkFreeMemory(vkDevice, vkIndexBufferMemory, nullptr);

		vkDestroyBuffer(vkDevice, vkVertexBuffer, nullptr);
		vkFreeMemory(vkDevice, vkVertexBufferMemory, nullptr);

		vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);

		for (auto& i : vkSwapChainFramebuffers) {
			vkDestroyFramebuffer(vkDevice, i, nullptr);
		}

		for (auto& i : vkSwapChainImageViews) {
			vkDestroyImageView(vkDevice, i, nullptr);
		}
		vkDestroyPipeline(vkDevice, vkGraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, nullptr);
		vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
		vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; i++) {
			vkUnmapMemory(vkDevice, uniformMem[i]);
			vkFreeMemory(vkDevice, uniformMem[i], nullptr);
			vkDestroyBuffer(vkDevice, uniformBuffer[i], nullptr);
		}

		vkDestroyDescriptorPool(vkDevice, vkDescPool, nullptr);

		vkDestroyDescriptorSetLayout(vkDevice, vkDescLayout, nullptr);

		vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
		vkDestroyDevice(vkDevice, nullptr);

		if (enableValidationLayers) {
			destroy_debug_utils_message_ext(vkInstance, &vkDebugMessenger, nullptr);
		}

		vkDestroyInstance(vkInstance, nullptr);
		glfwDestroyWindow(window);
		window = nullptr;
		glslang_finalize_process();
	}

	void Render::fill_vulkan_debug_message_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = vk_debug_callback;
		createInfo.pUserData = nullptr;
	}

	expected<vector<VkExtensionProperties>, WERROR> Render::check_extensions() {
		uint32_t count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		vector<VkExtensionProperties> extensions(count);
		vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

		spdlog::info("available extensions:");
		for (const auto& extension : extensions) {
			spdlog::info("\t{}", extension.extensionName);
		}

		return std::move(extensions);
	}

	expected<vector<VkLayerProperties>, WERROR> Render::check_validation_layers() {
		uint32_t count = 0;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		vector<VkLayerProperties> layers(count);
		vkEnumerateInstanceLayerProperties(&count, layers.data());

		spdlog::info("available validation layers:");
		for (const auto& layer : layers) {
			spdlog::info("\t{}", layer.layerName);
		}

		return std::move(layers);
	}

	Render::QueueFamilyIndices Render::find_queue_families(VkPhysicalDevice device) {
		QueueFamilyIndices indices{};

		uint32_t           count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

		vector<VkQueueFamilyProperties> queueFamilies(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilies.data());

		int idx = 0;
		for (const auto& i : queueFamilies) {
			if (i.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = idx;
			} else {
				if (VkBool32 presentSupport = false; vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, vkSurface, &presentSupport) == VK_SUCCESS) {
					if (presentSupport) {
						indices.presentFamily = idx;
					}
				}
			}

			if (indices.isComplete()) {
				break;
			}
			idx++;
		}
		return std::move(indices);
	}

	VKAPI_ATTR VkBool32 Render::vk_debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
	) {
		// if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		spdlog::info("Vulkan validation layer: {}", pCallbackData->pMessage);
		// }
		return VK_FALSE;
	}

	void Render::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto host = reinterpret_cast<Render*>(glfwGetWindowUserPointer(window));
		host->framebufferResized = true;

		// host->draw_window_frame();
		// host->draw_window_frame();
	}
	// 查找vulkan内的扩展函数
	VkResult create_debug_utils_message_ext(
		VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroy_debug_utils_message_ext(VkInstance instance, VkDebugUtilsMessengerEXT* pDebugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func) {
			func(instance, *pDebugMessenger, pAllocator);
		}
	}
}; // namespace Warp
