cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET x64-windows)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(WarpEngine)


if(WIN32)
	#add_compile_options("/source-charset:utf-8")
	#add_compile_options("/source-charset:utf-8")
endif()

set(EXT_PATH "${CMAKE_SOURCE_DIR}/Ext")


#LLVM
set(LLVM_DIR "${EXT_PATH}/llvm17/lib/cmake/llvm")
#find_package(LLVM REQUIRED)

#MIMALLOC
set(mimalloc_LIB_DIR "${EXT_PATH}/mimalloc/lib")
set(mimalloc_INCLUDE_DIR "${EXT_PATH}/mimalloc/include" "${EXT_PATH}/mimalloc/include/mimalloc")
set(mimalloc_LIBS "${mimalloc_LIB_DIR}/mimalloc-secure.lib")
file(COPY "${mimalloc_LIB_DIR}/mimalloc-secure.lib" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "${mimalloc_LIB_DIR}/mimalloc-secure.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "${mimalloc_LIB_DIR}/mimalloc-redirect.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#TBB
set(tbb_LIB_DIR "${EXT_PATH}/tbb/lib")
set(tbb_INCLUDE_DIR "${EXT_PATH}/tbb/include" "${EXT_PATH}/tbb/include/tbb")
set(tbb_LIBS "${tbb_LIB_DIR}/tbb12.lib")
file(COPY "${tbb_LIB_DIR}/tbb12.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)
find_package(glslang REQUIRED)
#find_package(Eigen3 REQUIRED)


add_subdirectory(Ext/volk)
add_subdirectory(Ext/SDL)

add_subdirectory(Source)