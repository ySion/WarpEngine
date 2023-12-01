#pragma once
#include "Graphics/GPUResource.h"


#include "Graphics/GPUResource.h"
#include "Graphics/GPUResourceBuilder.h"


namespace Warp::Render {

	class RenderResourceGroup {

	public:

		inline RenderResourceGroup() {

			m_shader_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUShader>>("core_shader_manager");

			m_descriptor_pool_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorPool>>("core_descriptor_pool_manager");

			m_descriptor_set_layout_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorSetLayout>>("core_descriptor_set_layout_manager");

			m_pipeline_layout_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUPipelineLayout>>("core_pipeline_layout_manager");

			m_renderpass_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPURenderPass>>("core_render_pass_manager");

			m_graphics_pipeline_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUGraphicsPipeline>>("core_graphics_pipeline_manager");

			m_fence_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFence>>("core_fence_manager");

			m_semahpore_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSemaphore>>("core_semaphore_manager");

			m_command_pool_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUCommandPool>>("core_command_pool_manager");

			m_buffer_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUBuffer>>("core_buffer_manager"); 

			m_image_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUImage>>("core_image_manager");

			m_swap_chain_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSwapChain>>("core_swap_chain_manager");

			m_frame_buf_mag = std::make_unique<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFrameBuffer>>("core_frame_buffer_manager");
		}

		inline ~RenderResourceGroup() = default;

		inline RenderResourceGroup(const RenderResourceGroup&) = delete;
		inline RenderResourceGroup& operator=(const RenderResourceGroup&) = delete;

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUShader>* get_shader_manager() const {
			return m_shader_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorPool>* get_descriptor_set_layout_manager() const {
			return m_descriptor_pool_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorSetLayout>* get_descriptor_set_manager() const {
			return m_descriptor_set_layout_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUPipelineLayout>* get_pipeline_layout_manager() const {
			return m_pipeline_layout_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPURenderPass>* get_render_pass_manager() const {
			return m_renderpass_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUGraphicsPipeline>* get_graphics_pipeline_manager() const {
			return m_graphics_pipeline_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFence>* get_fence_manager() const {
			return m_fence_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSemaphore>* get_semaphore_manager() const {
			return m_semahpore_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUCommandPool>* get_command_pool_manager() const {
			return m_command_pool_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUBuffer>* get_buffer_manager() const {
			return m_buffer_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUImage>* get_image_manager() const {
			return m_image_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSwapChain>* get_swap_chain_manager() const {
			return m_swap_chain_mag.get();
		}

		inline GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFrameBuffer>* get_frame_buffer_manager() const {
			return m_frame_buf_mag.get();
		}

	private:

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUShader>> m_shader_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorPool>> m_descriptor_pool_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUDescriptorSetLayout>> m_descriptor_set_layout_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUPipelineLayout>> m_pipeline_layout_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPURenderPass>> m_renderpass_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUGraphicsPipeline>> m_graphics_pipeline_mag;



		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFence>> m_fence_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSemaphore>> m_semahpore_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUCommandPool>> m_command_pool_mag;



		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUBuffer>> m_buffer_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUImage>> m_image_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUSwapChain>> m_swap_chain_mag;

		std::unique_ptr<GPU::GPUResourceManager<GPU::GPUResourceTypes::GPUFrameBuffer>> m_frame_buf_mag;
	};
	
}