#pragma once
#include <memory>
#include <ranges>

#include <type_traits>
#include <vector>

#include "VulkanFiles.h"
#include "Core/Log.h"
#include "Core/MIStl.h"

namespace Warp {
	namespace GPU {

		class GPUResource;

		template<typename T> class GPUResourceManager;


		struct GPUFactoryCreateInfo {
			std::vector<const char*> enable_instance_extension{};
			std::vector<const char*> enable_instance_layer{};
			std::vector<const char*> enable_device_extension{};
		};

		class GPUFactory {
		public:
			GPUFactory() = delete;
			~GPUFactory() = delete;

			template<class T> requires std::is_base_of_v<GPUResource, T>
			[[nodiscard]] static std::unique_ptr<GPUResourceManager<T>> make_manager(const MString& name) {
				return std::move(std::make_unique<GPUResourceManager<T>>(name));
			}


			static inline VkDevice get_device() {
				return m_vk_device_;
			}

			static inline VkInstance get_instance() {
				return m_vk_instance_;
			}

			static inline VkPhysicalDevice get_physical_device() {
				return m_vk_physical_device_;
			}

			static inline VkQueue get_queue() {
				return m_vk_queue_;
			}

			static inline VmaAllocator get_vma() {
				return m_vma_allocator_;
			}

			static inline uint32_t get_swap_chain_image_count() {
				return m_swapchain_image_count;
			}

			static inline uint32_t get_frame_command_buffer_count() {
				return m_frame_command_buffer;
			}

			static inline void manager_ptr_register(void* ptr) {
				LOG("[GPUFactory][ManagerPtr][Register] {}.", ptr);
				m_manager_ptr_register.insert(ptr);
			}

			static inline void manager_ptr_unregister(void* ptr) {
				if(m_manager_ptr_register.contains(ptr)){
					LOG("[GPUFactory][ManagerPtr][Unregister] {}.", ptr);
					m_manager_ptr_register.erase(ptr);
				}
			}

			static inline bool manager_ptr_is_register_available(void* ptr){
				return m_manager_ptr_register.contains(ptr);
			}

			//初始化引擎
			static void init(const GPUFactoryCreateInfo& create_info);

			static void exit();

		private:
			static inline VkInstance m_vk_instance_{ nullptr };
			static inline VkPhysicalDevice m_vk_physical_device_{ nullptr };
			static inline VkDevice m_vk_device_{ nullptr };
			static inline VkQueue m_vk_queue_{ nullptr };
			static inline VmaAllocator m_vma_allocator_{ nullptr };
			static inline uint32_t m_swapchain_image_count{ 0 };
			static inline uint32_t m_frame_command_buffer { 3 };
			static inline MSet<void*> m_manager_ptr_register{};
		};
	}
}
