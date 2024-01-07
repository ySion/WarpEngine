#pragma once

#include "VulkanFiles.h"

#include "GPUFactory.h"
#include "GPUResource.h"
#include "GPUResourceManager.h"
#include "GPUHelper.h"
#include "GPUShaderCompiler.h"

#include "Core/Utils.h"

#include "Graphics/Builder/GPUBufferBuilder.h"
#include "Graphics/Builder/GPUImageBuilder.h"
#include "Graphics/Builder/GPUSwapChainBuilder.h"
#include "Graphics/Builder/GPUFrameBufferBuilder.h"
#include "Graphics/Builder/GPUCommandPoolBuilder.h"
#include "Graphics/Builder/GPUFenceBuilder.h"
#include "Graphics/Builder/GPUSemaphoreBuilder.h"
#include "Graphics/Builder/GPURenderPassBuilder.h"
#include "Graphics/Builder/GPUGraphicsPipelineBuilder.h"
#include "Graphics/Builder/GPUPipelineLayoutBuilder.h"
#include "Graphics/Builder/GPUDescriptorPoolBuilder.h"
#include "Graphics/Builder/GPUDescriptorSetLayoutBuilder.h"

namespace Warp {
	namespace GPU {
		template<class T> struct GPUResourceBuildTemplate;

		template<typename T> class GPUResourceBuilder {};

		namespace GPUResourceTypes {
			using GPUImage = ::Warp::GPU::GPUImage;
			using GPUSwapChain = ::Warp::GPU::GPUSwapChain;
			using GPUCommandPool = ::Warp::GPU::GPUCommandPool;
			using GPUFence = ::Warp::GPU::GPUFence;
			using GPUSemaphore = ::Warp::GPU::GPUSemaphore;
			using GPURenderPass = ::Warp::GPU::GPURenderPass;
			using GPUGraphicsPipeline = ::Warp::GPU::GPUGraphicsPipeline;
			using GPUShader = ::Warp::GPU::GPUShader;
			using GPUPipelineLayout = ::Warp::GPU::GPUPipelineLayout;
			using GPUFrameBuffer = ::Warp::GPU::GPUFrameBuffer;
			using GPUBuffer = ::Warp::GPU::GPUBuffer;
			using GPUDescriptorPool = ::Warp::GPU::GPUDescriptorPool;
			using GPUDescriptorSetLayout = ::Warp::GPU::GPUDescriptorSetLayout;
		}


		//Frame Buffer具有自行创建Image和View的能力

		template<>
		class GPUResourceBuilder<GPUShader> {
			GPUResourceManager<GPUShader>* m_manager{};
		public:
			using self_type = GPUResourceBuilder<GPUShader>;
			struct CreateInfo {
				MString m_name;
				VkShaderModuleCreateInfo m_module_create_info{};
			};

			GPUResourceBuilder(GPUResourceManager<GPUShader>* manager, const MString& name) : m_manager(manager)
			{
				create_info.m_name = name;
			}

			GPUShader* make_from_file(const MString& file_name, EShLanguage stage) {
				if (create_info.m_name.empty()) {
					LOGE("[GPUResourceBuilder<{}>] file name is empty.", typeid(GPUShader).name());
					return nullptr;
				}

				MString codes{};
				try {
					codes = Warp::read_text_file(file_name);
				} catch (...) {
					LOGE("[GPUResourceBuilder<{}>] can't read from file: {}.", typeid(GPUShader).name(), file_name);
					return nullptr;
				}

				if (codes.empty()) {
					LOGE("[GPUResourceBuilder<{}>] file is empty.", typeid(GPUShader).name());
					return nullptr;
				}

				return make_from_string(codes, stage);
			}

			//always replaced
			GPUShader* make_from_string(const MString& codes, EShLanguage stage) {
				if (create_info.m_name.empty()) { return nullptr; }

				if (codes.empty()) { return nullptr; }

				
				MVector<uint32_t> spirv{};
				const MString errs = ShaderCompiler::compile_shader_from_string(create_info.m_name, codes, stage, spirv);
				if(!errs.empty())
				{
					LOGE("[GPUResourceBuilder<{}>] Shader Compile, {}.", typeid(GPUShader).name(), errs);
				}

				if (spirv.empty()) {
					LOGE("[GPUResourceBuilder<{}>] Shader Compile success, but null spriv.", typeid(GPUShader).name());
					return nullptr;
				}

				create_info.m_module_create_info = {
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.codeSize = spirv.size() * sizeof(uint32_t),
					.pCode = spirv.data()
				};

				VkShaderModule shader_module{};

				if (VK_SUCCESS != vkCreateShaderModule(GPUFactory::get_device(), &create_info.m_module_create_info, nullptr, &shader_module)) {
					LOGE("[GPUResourceBuilder<{}>] Create shader module failed.", typeid(GPUShader).name());
					return nullptr;
				}
				const auto obj = new GPUShader(create_info.m_name);

				obj->m_code = codes;
				obj->m_stage = stage;
				obj->m_spirv = MVector<uint32_t>(spirv.begin(), spirv.end());
				obj->m_module = shader_module;

				m_manager->add(obj);
				return obj;
			}

		private:
			CreateInfo create_info{};
		};
	}
}
