#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"


WARP_TYPE_NAME_2(Gpu, DescriptorSet);

namespace Warp::Gpu {

	class DescriptorPool;

	class DescriptorSet : public Inherit<DescriptorSet, Object> {
	public:
		DescriptorSet(DescriptorPool* pool, VkDescriptorSet set);

		~DescriptorSet() override = default;

		void update_image(uint32_t binding, uint32_t array_index, VkDescriptorType type, uint32_t count, const VkDescriptorImageInfo* image_info) const;

		void update_buffer(uint32_t binding, uint32_t array_index, VkDescriptorType type, uint32_t count, const VkDescriptorBufferInfo* buffer_info) const;

		inline DescriptorPool* get_pool() const { return _pool; }

		inline VkDescriptorSet vk() const { return _set; }

		operator VkDescriptorSet() const { return _set; }

	private:
		VkDescriptorSet _set{};
		
		DescriptorPool* _pool{};
	};
};