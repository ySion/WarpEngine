#pragma once

#include "Core/Object.hpp"
#include "Core/Inherit.hpp"


#include "Instance.hpp"
#include "Device.hpp"
#include "Window.hpp"
#include "CommandSubmitQueue.hpp"
#include "Image.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "PipelineLayout.hpp"
#include "ShaderModule.hpp"
#include "Buffer.hpp"

WARP_TYPE_NAME_2(Gpu, Context)

namespace Warp::Gpu {

	using Names = MVector<const char*>;

	class Context : public Inherit<Context, Object> {
	public:

		Context();

		~Context() override = default;

		inline Instance* get_instance() const { return _instance.get(); }

		inline Device* get_device() const { return _device.get(); }

		inline CommandSubmitQueue* get_command_submit_queue() const { return _command_submit_queue.get(); }

		std::unique_ptr<Window> create_window(const MString& title, int w, int h, uint32_t flag);

		std::unique_ptr<Image> create_image(VkFormat format,
			VkExtent3D extent,
			VkImageUsageFlags usage,
			VmaAllocationCreateFlags memory_flags,
			VkImageType imageType,
			uint32_t mip_levels = 1,
			uint32_t arrayLayers = 1,
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
			VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) const;

		//VK_FORMAT_D32_SFLOAT
		std::unique_ptr<Image> create_image_depth(VkExtent3D extent,
			VmaAllocationCreateFlags memory_flags = 0,
			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			uint32_t mip_levels = 1,
			uint32_t arrayLayers = 1,
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
			VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) const;

		//R8G8BA8_SRGB
		std::unique_ptr<Image> create_image_color(VkExtent3D extent,
			VmaAllocationCreateFlags memory_flags = 0,
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			uint32_t mip_levels = 1,
			uint32_t arrayLayers = 1,
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
			VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) const;

		std::unique_ptr<Buffer> create_buffer(VkDeviceSize size,
						VkBufferUsageFlags usage,
						VmaAllocationCreateFlags memory_flags = 0,
						VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
						VkDeviceSize alignment = 0) const;

		//need after parameter set and compile
		std::unique_ptr<DescriptorPool> create_descriptor_pool(uint32_t maxSets) const;

		//need after parameter set and compile
		std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(VkDescriptorBindingFlags flags = 0) const;

		//need after parameter set and compile
		std::unique_ptr<PipelineLayout> create_pipeline_layout() const;

		//need after parameter set and compile
		std::unique_ptr<RenderPass> create_render_pass() const;

		std::unique_ptr<GraphicsPipeline> create_graphics_pipeline(GraphicsPipelineState* state, RenderPass* renderpass, PipelineLayout* layout) const;

		std::unique_ptr<ShaderModule> create_shader_module(ShaderType shader_type, const MVector<uint32_t>& spirv) const;
	private:
		std::unique_ptr<Instance> _instance{};

		std::unique_ptr<Device> _device{};

		std::unique_ptr<CommandSubmitQueue> _command_submit_queue{};
	};
}
