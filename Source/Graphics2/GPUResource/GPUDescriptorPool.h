#pragma once


#include "GPUResourceBase.h"
#include "Graphics/GPUResource/GPUDescriptorSetLayout.h"

namespace Warp {
	namespace GPU {

		class GPUDescriptorPool final : public GPUResource {
		public:
			inline GPUDescriptorPool(const MString& name) : GPUResource("descriptor_pool", name) {
				m_descriptor_sets.reserve(32);
			}

			inline ~GPUDescriptorPool() override {
				vkDestroyDescriptorPool(GPUFactory::get_device(), m_pool, nullptr);
			}

			VkDescriptorSet make_descriptor_set(const GPUDescriptorSetLayout * layout) {
				VkDescriptorSetAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = m_pool;
				alloc_info.descriptorSetCount = 1;
				alloc_info.pSetLayouts = &layout->m_layout;

				VkDescriptorSet descriptor_set{};
				if (vkAllocateDescriptorSets(GPUFactory::get_device(), &alloc_info, &descriptor_set) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate descriptor set!");
				}
				m_descriptor_sets.push_back(descriptor_set);
				return descriptor_set;
			}

			MVector<VkDescriptorSet> make_multi_descriptor_set(const GPUDescriptorSetLayout* layout, uint32_t count) {
				MVector<VkDescriptorSet> descriptor_sets{ count };

				VkDescriptorSetAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = m_pool;
				alloc_info.descriptorSetCount = count;
				alloc_info.pSetLayouts = &layout->m_layout;

				if (vkAllocateDescriptorSets(GPUFactory::get_device(), &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate descriptor set!");
				}

				m_descriptor_sets.insert(m_descriptor_sets.end(), descriptor_sets.begin(), descriptor_sets.end());
				return descriptor_sets;
			}

			void clear_descriptor_sets() {
				vkResetDescriptorPool(GPUFactory::get_device(), m_pool, 0);
				m_descriptor_sets.clear();
			}

			//info
			VkDescriptorPool m_pool{};
			uint32_t m_max_sets{};
			VkDescriptorPoolCreateFlags m_flags{};
			MVector<VkDescriptorPoolSize> m_pool_sizes{};

			//sets
			MVector<VkDescriptorSet> m_descriptor_sets{};
		};
	}

}
