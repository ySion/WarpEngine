#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"


WARP_TYPE_NAME_2(Gpu, DescriptorSetLayout);

namespace Warp::Gpu{
	class Device;

	class DescriptorSetLayout : public Inherit<DescriptorSetLayout, Object> {
	public:
		DescriptorSetLayout(Device* device, VkDescriptorBindingFlags flags);
		~DescriptorSetLayout() override;

		inline DescriptorSetLayout& add_binding(const VkDescriptorSetLayoutBinding& binding) {
			_bindings.push_back(binding);
			return *this;
		}

		inline DescriptorSetLayout& add_binding(
			uint32_t              binding,
			VkDescriptorType      descriptorType,
			uint32_t              descriptorCount,
			VkShaderStageFlags    stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,
			const VkSampler* pImmutableSamplers = nullptr) {
			add_binding({ binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers });
			return *this;
		}

		VkResult compile();

		inline Device* get_device() const { return _device; }

		inline VkDescriptorSetLayout vk() const { return _set_layout; }

		operator VkDescriptorSetLayout() const { return _set_layout; }
	private:
		VkDescriptorSetLayout _set_layout;

		Device* _device{};

		VkDescriptorBindingFlags _flags{};

		MVector<VkDescriptorSetLayoutBinding> _bindings {};
	};

}