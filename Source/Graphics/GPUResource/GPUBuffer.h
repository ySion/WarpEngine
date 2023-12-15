#pragma once

#include "GPUResourceBase.h"

#include "Core/Log.h"

namespace Warp {

	namespace GPU {

		class GPUBufferView : public WObject {
		public:
			struct Deleter {
				constexpr void operator()(const GPUBufferView* ptr) const {
					if (ptr) {
						if(ptr->m_buffer_view) {
							vkDestroyBufferView(GPUFactory::get_device(), ptr->m_buffer_view, nullptr);
						}
						delete ptr;
					}
				}
			};

			VkBuffer m_buffer{};
			VkBufferView m_buffer_view{};
			VkFormat m_format{};
			VkDeviceSize m_offest{};
			VkDeviceSize m_range{};
			VkBufferViewCreateFlags m_flags{};
		};

		class GPUBuffer : public GPUResource {
		public:
			inline GPUBuffer(const MString& name) : GPUResource("buffer", name) {}

			inline ~GPUBuffer() {
				m_views.clear();
				vmaDestroyBuffer(GPUFactory::get_vma(), m_buffer, m_memory);
			}

			void clear_views();

			GPUBufferView* add_view(VkFormat format, VkDeviceSize offest, VkDeviceSize range, VkBufferViewCreateFlags flags = 0);

			GPUBufferView* add_view(const VkBufferViewCreateInfo& create_info);

			VkBuffer m_buffer{};
			VkDeviceSize m_size{};
			VmaAllocation m_memory{};
			VkBufferUsageFlags m_usage{};
			MVector<std::unique_ptr<GPUBufferView, GPUBufferView::Deleter>> m_views{};
		};
	}
}