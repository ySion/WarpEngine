#include "DescriptorPool.hpp"

#include "DescriptorSetLayout.hpp"
#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp::Gpu;

DescriptorPool::DescriptorPool(Device* device, uint32_t max_desc_sets)
	: _device(device), _max_desc_sets(max_desc_sets)
{}

DescriptorPool::~DescriptorPool() {
	if(_descriptor_pool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(*_device, _descriptor_pool, nullptr);
	}
}

DescriptorSet* DescriptorPool::allocate_set(const DescriptorSetLayout* layout) {
	
	std::array arr = { layout->vk() };
	const VkDescriptorSetAllocateInfo ai = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = _descriptor_pool,
		.descriptorSetCount = to_u32(arr.size()),
		.pSetLayouts = arr.data()
	};
	VkDescriptorSet set;
	if (VkResult result = vkAllocateDescriptorSets(*_device, &ai, &set); result != VK_SUCCESS) {
		MString msg = MString::format("Failed to allocate descriptor set: {}, {}, ", static_cast<int>(result), msg_map_VkResult(result));
		error(msg);
		return nullptr;
	}

	return _sets.emplace_back(new DescriptorSet(this, set)).get();
}

VkResult DescriptorPool::compile() {

	VkDescriptorPoolCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = _max_desc_sets,
		.poolSizeCount = to_u32(_pool_sizes.size()),
		.pPoolSizes = _pool_sizes.data()
	};

	if(VkResult result = vkCreateDescriptorPool(*_device, &ci, nullptr, &_descriptor_pool); result != VK_SUCCESS) {
		MString msg = MString::format("Failed to create descriptor pool: {}, {}, ", static_cast<int>(result), msg_map_VkResult(result));
		error(msg);
		return result;
	}

	return VK_SUCCESS;
}
