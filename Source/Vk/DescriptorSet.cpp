#include "DescriptorSet.hpp"

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"

#include "Core/Logger.hpp"

using namespace Warp::Gpu;


DescriptorSet::DescriptorSet(DescriptorPool* pool, VkDescriptorSet set) : _set(set), _pool(pool)
{
}

void DescriptorSet::update_image(uint32_t binding, uint32_t array_index, VkDescriptorType type, uint32_t count,
                                 const VkDescriptorImageInfo* image_info) const
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = _set;
	write.dstBinding = binding;
	write.dstArrayElement = array_index;
	write.descriptorCount = count;
	write.descriptorType = type;
	write.pBufferInfo = nullptr;
	write.pImageInfo = image_info;

	vkUpdateDescriptorSets(*_pool->get_device(), 1, &write, 0, nullptr);
}

void DescriptorSet::update_buffer(uint32_t binding, uint32_t array_index, VkDescriptorType type, uint32_t count,
	const VkDescriptorBufferInfo* buffer_info) const
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = _set;
	write.dstBinding = binding;
	write.dstArrayElement = array_index;
	write.descriptorCount = count;
	write.descriptorType = type;
	write.pBufferInfo = buffer_info;
	write.pImageInfo = nullptr;

	vkUpdateDescriptorSets(*_pool->get_device(), 1, &write, 0, nullptr);
}
