#include "GPUFactory.h"

#include "VulkanFiles.h"

namespace Warp {
	namespace GPU {

		void GPUFactory::init(const GPUFactoryCreateInfo& create_info) {
			SDL_Init(SDL_INIT_EVERYTHING);

			volkInitialize();
			glslang::InitializeProcess();
			VkApplicationInfo vk_application_info{};
			vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			vk_application_info.pApplicationName = "Warp Engine";
			vk_application_info.apiVersion = VK_API_VERSION_1_3;
			vk_application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			vk_application_info.pEngineName = "Warp Engine";
			vk_application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

			VkInstanceCreateInfo vk_instance_create_info{};
			vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			vk_instance_create_info.enabledExtensionCount = create_info.enable_instance_extension.size();
			vk_instance_create_info.ppEnabledExtensionNames = create_info.enable_instance_extension.data();
			vk_instance_create_info.enabledLayerCount = create_info.enable_instance_layer.size();
			vk_instance_create_info.ppEnabledLayerNames = create_info.enable_instance_layer.data();
			vk_instance_create_info.pApplicationInfo = &vk_application_info;

			if (vkCreateInstance(&vk_instance_create_info, nullptr, &m_vk_instance_) != VK_SUCCESS) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan init failed", "Vulkan instance create failed", nullptr);
				std::terminate();
			}

			volkLoadInstance(m_vk_instance_);

			//pick gpu
			{
				uint32_t physical_device_count = 0;
				vkEnumeratePhysicalDevices(get_instance(), &physical_device_count, nullptr);
				std::vector<VkPhysicalDevice> vk_physical_devices{ physical_device_count };
				vkEnumeratePhysicalDevices(get_instance(), &physical_device_count, vk_physical_devices.data());


				uint32_t pick_index = 0;
				uint32_t temp_score = 0;
				for (auto [i, c] : std::ranges::views::enumerate(vk_physical_devices)) {
					VkPhysicalDeviceProperties vk_physical_device_properties{};
					vkGetPhysicalDeviceProperties(c, &vk_physical_device_properties);

					uint32_t current_score = 0;
					if (vk_physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
						current_score += 1000;
					}
					current_score += vk_physical_device_properties.limits.maxImageDimension2D;
					if (temp_score < current_score) {
						temp_score = current_score;
						pick_index = i;
					}
				}

				m_vk_physical_device_ = vk_physical_devices[pick_index];
			}

			if (m_vk_physical_device_ == nullptr) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Video card support", "No available video card found", nullptr);
				std::terminate();
			}


			float queue_prior = 1.0;
			VkDeviceQueueCreateInfo vk_device_queue_create_info{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = 0,
				.queueCount = 1,
				.pQueuePriorities = &queue_prior
			};

			VkPhysicalDeviceFeatures vk_physical_device_features{};
			vkGetPhysicalDeviceFeatures(m_vk_physical_device_, &vk_physical_device_features);


			VkDeviceCreateInfo vk_device_create_info{
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueCreateInfoCount = 1,
				.pQueueCreateInfos = &vk_device_queue_create_info,
				.enabledLayerCount = 0,
				.ppEnabledLayerNames = nullptr,
				.enabledExtensionCount = (uint32_t)create_info.enable_device_extension.size(),
				.ppEnabledExtensionNames = create_info.enable_device_extension.data(),
				.pEnabledFeatures = &vk_physical_device_features
			};


			if (vkCreateDevice(m_vk_physical_device_, &vk_device_create_info, nullptr, &m_vk_device_) != VK_SUCCESS) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan init failed", "Vulkan device create failed", nullptr);
				std::terminate();
			}


			volkLoadDevice(m_vk_device_);

			{
				vkGetDeviceQueue(m_vk_device_, 0, 0, &m_vk_queue_);
				if (m_vk_queue_ == nullptr) {
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan init failed", "Vulkan queue get failed", nullptr);
					std::terminate();
				}
			}


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
			vma_allocator_create_info.device = m_vk_device_;
			vma_allocator_create_info.physicalDevice = m_vk_physical_device_;
			vma_allocator_create_info.instance = m_vk_instance_;
			vma_allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_3;
			vma_allocator_create_info.pVulkanFunctions = &vma_vulkan_func;

			if (vmaCreateAllocator(&vma_allocator_create_info, &m_vma_allocator_) != VK_SUCCESS) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan init failed", "Vulkan vma allocator create failed", nullptr);
				std::terminate();
			}


			VkSurfaceKHR temp_surface{};
			SDL_Window* window = SDL_CreateWindow("surface_getter", 360, 360, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
			if(!SDL_Vulkan_CreateSurface(window, m_vk_instance_,nullptr, &temp_surface)) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan init failed", "Vulkan surface create failed", nullptr);
				std::terminate();
			}

			VkSwapchainCreateInfoKHR temp_swap_chain_create_info{
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.pNext = nullptr,
				.flags = 0,
				.surface = temp_surface,
				.minImageCount = 3,
				.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
				.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
				.imageExtent = {360,360},
				.imageArrayLayers = 1,
				.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices = nullptr,
				.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
				.clipped = true,
				.oldSwapchain = nullptr 
			};

			VkSwapchainKHR temp_swap_chain{};
			vkCreateSwapchainKHR(GPUFactory::get_device(), &temp_swap_chain_create_info, nullptr, &temp_swap_chain);

			vkGetSwapchainImagesKHR(GPUFactory::get_device(), temp_swap_chain, &m_swapchain_image_count, nullptr);

			vkDestroySwapchainKHR(GPUFactory::get_device(), temp_swap_chain, nullptr);
			vkDestroySurfaceKHR(m_vk_instance_, temp_surface, nullptr);
			SDL_DestroyWindow(window);
		}

		void GPUFactory::exit() {
			if (m_vma_allocator_) { vmaDestroyAllocator(m_vma_allocator_); }
			if (m_vk_device_) { vkDestroyDevice(m_vk_device_, nullptr); }
			if (m_vk_instance_) { vkDestroyInstance(m_vk_instance_, nullptr); }
		}
	}
}