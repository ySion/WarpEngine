#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUCommandPool.h"
#include "Graphics/GPUResource/GPUCommandBuffer.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUCommandBuffer> {
		public:
			using target_type = GPUCommandBuffer;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkCommandBufferAllocateInfo ci_command_buffer{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.pNext = nullptr,
					.commandPool = VK_NULL_HANDLE,
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1,
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_configs(const GPUCommandPool* pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
				create_info.ci_command_buffer.level = level;
				create_info.ci_command_buffer.commandPool = pool->m_command_pool;
				return *this;
			}

			self_type& set_configs(VkCommandPool pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
				create_info.ci_command_buffer.level = level;
				create_info.ci_command_buffer.commandPool = pool;
				return *this;
			}

			target_type* make() {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, this type can't be replaced make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkCommandBuffer cmdbuffer{};
					if (res = vkAllocateCommandBuffers(GPUFactory::get_device(), &create_info.ci_command_buffer, &cmdbuffer);
						res != VK_SUCCESS) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					auto obj_ptr = std::make_unique<target_type>(create_info.name);

					obj_ptr->m_command_buffer = cmdbuffer;
					obj_ptr->m_command_pool = create_info.ci_command_buffer.commandPool;
					obj_ptr->m_level = create_info.ci_command_buffer.level;

					obj_ptr->m_submit_info = {
						.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
						.pNext = nullptr,
						.waitSemaphoreCount = 0,
						.pWaitSemaphores = nullptr,
						.pWaitDstStageMask = nullptr,
						.commandBufferCount = 1,
						.pCommandBuffers = &obj_ptr->m_command_buffer,
						.signalSemaphoreCount = 0,
						.pSignalSemaphores = nullptr
					};

					const auto temp_ptr = obj_ptr.get();
					m_manager->add(std::move(obj_ptr));
					return temp_ptr;
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

			MVector<target_type*> make_multi(uint32_t count) {
				MVector<target_type*> result;
				MString name = create_info.name;
				for (uint32_t i = 0; i < count; ++i) {
					if (!create_info.name.empty()) {
						create_info.name = to_MString(std::format("{}_{}", name, i));
					}
					if (auto obj = make()) {
						result.push_back(obj);
					}
				}
				return result;
			}

			void make_and_execute_single_time_command_buffer(const std::function<void(GPUCommandBuffer& command_buffer)>& call_back) {

				if (create_info.ci_command_buffer.commandPool == VK_NULL_HANDLE || !call_back) {
					return;
				}

				VkCommandBuffer vk_command_buffer{};

				create_info.ci_command_buffer.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				create_info.ci_command_buffer.commandBufferCount = 1;

				if (auto res = vkAllocateCommandBuffers(GPUFactory::get_device(), &create_info.ci_command_buffer, &vk_command_buffer);
					res != VK_SUCCESS || !vk_command_buffer) {
					return;
				}

				const std::unique_ptr<GPUCommandBuffer> command_buffer_warper_ptr = std::make_unique<GPUCommandBuffer>("@single_time_command_buffer");

				command_buffer_warper_ptr->m_command_buffer = vk_command_buffer;
				command_buffer_warper_ptr->m_command_pool = create_info.ci_command_buffer.commandPool;
				command_buffer_warper_ptr->m_level = create_info.ci_command_buffer.level;
				command_buffer_warper_ptr->begin_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

				call_back(*command_buffer_warper_ptr);

				command_buffer_warper_ptr->end_command_buffer();
				command_buffer_warper_ptr->submit();
			}

		private:
			CreateInfo create_info{};
			GPUResourceManager<target_type>* m_manager{};
		};
	}
}