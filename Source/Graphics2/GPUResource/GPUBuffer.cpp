#include "GPUBuffer.h"


namespace Warp {

	namespace GPU {

		void GPUBuffer::clear_views() {
			m_views.clear();
		}

		GPUBufferView* GPUBuffer::add_view(VkFormat format, VkDeviceSize offest, VkDeviceSize range,
						   VkBufferViewCreateFlags flags) {
			const VkBufferViewCreateInfo ci = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = flags,
				.buffer = m_buffer,
				.format = format,
				.offset = offest,
				.range = range,
			};
			return add_view(ci);
		}

		GPUBufferView* GPUBuffer::add_view(const VkBufferViewCreateInfo& create_info) {

			try {
				VkBufferView buffer_view{};

				if (vkCreateBufferView(GPUFactory::get_device(), &create_info, nullptr, &buffer_view) != VK_SUCCESS) {
					return nullptr;
				}

				auto* temp = new GPUBufferView{
					.m_buffer = create_info.buffer,
					.m_buffer_view = buffer_view,
					.m_format = create_info.format,
					.m_offest = create_info.offset,
					.m_range = create_info.range,
					.m_flags = create_info.flags,
				};

				m_views.emplace_back(temp);

				return temp;
			} catch (...) {
				return nullptr;
			}
		}
	}
}