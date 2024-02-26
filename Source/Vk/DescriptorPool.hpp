#pragma once

#include "VkBase.hpp"
#include "Device.hpp"
#include "DescriptorSet.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, DescriptorPool);

namespace Warp::Gpu {


	class DescriptorSetLayout;
	class DescriptorPool : public Inherit<DescriptorPool, Object> {
	public:
		DescriptorPool(Device* device, uint32_t max_desc_sets);

		~DescriptorPool() override;

		inline DescriptorPool& add_pool_size(VkDescriptorType type, uint32_t count) {
			_pool_sizes.push_back({ type, count });
			return *this;
		}

		DescriptorSet* allocate_set(const DescriptorSetLayout* layout);

		VkResult compile();

		inline Device* get_device() const { return _device; }

		inline VkDescriptorPool vk() const { return _descriptor_pool; }

		operator VkDescriptorPool() const { return _descriptor_pool; }
	private:
		Device* _device {};
		VkDescriptorPool _descriptor_pool {};
		uint32_t _max_desc_sets {};
		MVector<VkDescriptorPoolSize> _pool_sizes {};
		MVector<std::unique_ptr<DescriptorSet>> _sets{};
	};
}