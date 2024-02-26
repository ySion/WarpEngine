#include "DescriptorSetLayout.hpp"

#include "Device.hpp"

#include "Core/Logger.hpp"

using namespace Warp::Gpu;

DescriptorSetLayout::DescriptorSetLayout(Device* device, VkDescriptorBindingFlags flags)
	: _device(device),  _flags(flags) {}


DescriptorSetLayout::~DescriptorSetLayout() {
	if(_set_layout != VK_NULL_HANDLE){
		vkDestroyDescriptorSetLayout(*_device, _set_layout, nullptr);
	}
}

VkResult DescriptorSetLayout::compile() {

	MVector<VkDescriptorBindingFlags> binding_flags(_bindings.size(), _flags);

	VkDescriptorSetLayoutBindingFlagsCreateInfo flag_ci = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.pNext = nullptr,
		.bindingCount = to_u32(_bindings.size()),
		.pBindingFlags = binding_flags.data()
	};

	const VkDescriptorSetLayoutCreateInfo dsl_ci {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &flag_ci,
		.bindingCount = to_u32(_bindings.size()),
		.pBindings = _bindings.data()
	};


	if(VkResult res = vkCreateDescriptorSetLayout(*_device, &dsl_ci, nullptr, &_set_layout); res != VK_SUCCESS) {
		MString msg = MString::format("Failed to create descriptor set layout: {}, {}.", static_cast<int>(res), msg_map_VkResult(res));
		error(msg);
		return res;
	}

	return VK_SUCCESS;
}

