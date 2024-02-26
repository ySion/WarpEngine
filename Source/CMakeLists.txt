set(WpEngine 
	"Core/Object.cpp"
	"Core/Visitor.cpp"

	"Core/Hash/xxh_x86dispatch.c"
	"Core/Hash/xxhash.c"

	"Core/Hash/xxh3.h"
	"Core/Hash/xxh_x86dispatch.h"
	"Core/Hash/xxhash.h"

	"Core/lmdb/lmdb.h"
	"Core/lmdb/mdb.c"
	"Core/lmdb/midl.c"

	"Core/Clipper2/clipper.engine.cpp"
	"Core/Clipper2/clipper.offset.cpp"
	"Core/Clipper2/clipper.rectclip.cpp"

	#=====================================================

	"Vk/Vma/vma.cpp"
	"Vk/VkBase.cpp"
	"Vk/Context.cpp"
	"Vk/Instance.cpp"
	"Vk/Queue.cpp"
	"Vk/Device.cpp"
	"Vk/CommandPool.cpp"
	"Vk/CommandBuffer.cpp"
	"Vk/SwapChain.cpp"
	"Vk/Window.cpp"
	"Vk/CommandSubmitQueue.cpp"
	"Vk/RenderPass.cpp"
	"Vk/GlslCompiler.cpp"
	"Vk/GraphicsPipeline.cpp"
	"Vk/GraphicsPipelineState.cpp"
	"Vk/DescriptorSetLayout.cpp"
	"Vk/PipelineLayout.cpp"
	"Vk/DescriptorPool.cpp"
	"Vk/DescriptorSet.cpp"
	"Vk/ImageView.cpp"
	"Vk/Buffer.cpp"
	"Vk/Image.cpp"
	"Vk/ShaderModule.cpp"

	"Vk/VkBase.hpp"
	"Vk/Context.hpp"
	"Vk/Instance.hpp"
	"Vk/Queue.hpp"
	"Vk/Device.hpp"
	"Vk/CommandPool.hpp"
	"Vk/CommandBuffer.hpp"
	"Vk/SwapChain.hpp"
	"Vk/Window.hpp"
	"Vk/CommandSubmitQueue.hpp"
	"Vk/RenderPass.hpp"
	"Vk/GlslCompiler.hpp"
	"Vk/GraphicsPipeline.hpp"
	"Vk/GraphicsPipelineState.hpp"
	"Vk/DescriptorSetLayout.hpp"
	"Vk/PipelineLayout.hpp"
	"Vk/DescriptorPool.hpp"
	"Vk/DescriptorSet.hpp"
	"Vk/ImageView.hpp"
	"Vk/Buffer.hpp"
	"Vk/Image.hpp"
	"Vk/ShaderModule.hpp"

	#=====================================================

	"Graphics/RenderStage.cpp"
	"Graphics/RenderTaskList.cpp"
	"Graphics/RenderResource.cpp"
	"Graphics/MaterialSource.cpp"
	"Graphics/MaterialInfo.cpp"
	"Graphics/MaterialParameter.cpp"
	"Graphics/MaterialShader.cpp"
	"Graphics/MaterialInstance.cpp" 

	"Graphics/RenderStage.hpp"
	"Graphics/RenderTaskList.hpp"
	"Graphics/RenderResource.hpp"
	"Graphics/MaterialSource.hpp"
	"Graphics/MaterialInfo.hpp"
	"Graphics/MaterialParameter.hpp"
	"Graphics/MaterialShader.hpp"
	"Graphics/MaterialInstance.hpp" 

	#=====================================================

	"Data/Data.cpp"
	"Data/DataLayoutStructureDescriptorParser.cpp"
	"Data/DataLayoutStructureDescriptorPack.cpp"
	"Data/DataLayoutStructureDescriptor.cpp"
	"Data/DataLayoutUnorderedBlockVector.cpp"
	"Data/DataUnion.cpp"
	"Data/DataTypes.cpp"
	"Data/DataBuffer.cpp" 
	"Data/DataBufferString.cpp" 
	"Data/DataBufferVector.cpp" 
	"Data/DataBufferUnorderedUniformVector.cpp" 
	"Data/DataLayout.cpp"
	"Data/DataLayoutUnorderedVector.cpp"
	"Data/DataLayoutUnorderedUniformChunkVector.cpp"

	"Data/Data.hpp"
	"Data/DataLayoutStructureDescriptorParser.hpp"
	"Data/DataLayoutStructureDescriptorPack.hpp"
	"Data/DataLayoutStructureDescriptor.hpp"
	"Data/DataLayoutUnorderedBlockVector.hpp"
	"Data/DataUnion.hpp"
	"Data/DataTypes.hpp"
	"Data/DataBuffer.hpp" 
	"Data/DataBufferString.hpp" 
	"Data/DataBufferVector.hpp" 
	"Data/DataBufferUnorderedUniformVector.hpp" 
	"Data/DataLayout.hpp"
	"Data/DataLayoutUnorderedVector.hpp" 
	"Data/DataLayoutUnorderedUniformChunkVector.hpp"

	#=====================================================

	"NodeSystem/NodeSystem.hpp"
	"NodeSystem/ContextDescriptor.hpp" 
	"NodeSystem/NodeDescriptor.hpp" 
	"NodeSystem/PinDescriptor.hpp"

	"NodeSystem/NodeSystem.cpp"
	"NodeSystem/ContextDescriptor.cpp" 
	"NodeSystem/NodeDescriptor.cpp" 
	"NodeSystem/PinDescriptor.cpp"

	#=====================================================
	
	"DensifySystem/Entity.cpp"
	"DensifySystem/Entity.hpp"
	"DensifySystem/Collection.hpp"
	"DensifySystem/Collection.cpp"

	"Managers/DensifySystemManager.cpp"
	"Managers/DensifySystemManager.hpp"
	"Managers/DataLayoutManager.hpp"
	"Managers/DataLayoutManager.cpp"

	#=====================================================
	
	"Engine/Window.cpp"
	"Events/WindowEvent.cpp"
	"Engine/EngineContext.cpp"
	
	"NewMain.cpp"
)

list(APPEND ALL_SRC ${WpEngine})

add_executable(WarpEngine ${ALL_SRC} )

add_definitions(${LLVM_DEFINITIONS})

target_include_directories(WarpEngine PRIVATE "." 
	${LLVM_INCLUDE_DIRS}
	${mimalloc_INCLUDE_DIR}
	${tbb_INCLUDE_DIR}
)

#llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(WarpEngine
    glm::glm
    ${mimalloc_LIBS}
    ${tbb_LIBS}
    spdlog::spdlog
    #Eigen3::Eigen
    glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV
    #${llvm_libs}
    SDL3::SDL3
    volk::volk
)
target_precompile_headers(WarpEngine PRIVATE 
   
)

target_compile_options(WarpEngine PUBLIC "/Zc:__cplusplus" "/source-charset:utf-8")
#target_compile_options(WarpEngine PUBLIC "/source-charset:utf-8")
