#pragma once

#include "volk.h"

#include <string>
#include <vector>


namespace Warp {
	namespace GPU {

		constexpr  bool is_depth_only_format(VkFormat format) {
			return format == VK_FORMAT_D16_UNORM ||
				format == VK_FORMAT_D32_SFLOAT;
		}

		constexpr bool is_stencil_only_format(VkFormat format) {
			return format == VK_FORMAT_S8_UINT;
		}

		constexpr bool is_depth_stencil_format(VkFormat format) {
			return format == VK_FORMAT_D16_UNORM_S8_UINT ||
				format == VK_FORMAT_D24_UNORM_S8_UINT ||
				format == VK_FORMAT_D32_SFLOAT_S8_UINT;
		}

		constexpr bool is_depth_or_stencil_format(VkFormat format) {
			return is_depth_only_format(format) || is_depth_stencil_format(format);
		}


		//stencil, depth, depth_stencil, color
		constexpr VkImageLayout get_image_layout(VkFormat format) {
			if (is_depth_stencil_format(format)) {
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else if (is_stencil_only_format(format)) {
				return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else if (is_depth_only_format(format)) {
				return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			}
			else {
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
		}

		//stencil, depth, depth_stencil, color

		constexpr VkImageLayout get_image_layout_read_only(VkFormat format) {
			if (is_depth_stencil_format(format)) {
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}
			else if (is_stencil_only_format(format)) {
				return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
			}
			else if (is_depth_only_format(format)) {
				return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
			}
			else {
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}

		constexpr const char* get_vk_result_string(VkResult res)
		{
			switch (res)
			{
				case VK_SUCCESS : return "VK_SUCCESS";
				case VK_NOT_READY : return "VK_NOT_READY";
				case VK_TIMEOUT : return "VK_TIMEOUT";
				case VK_EVENT_SET : return "VK_EVENT_SET";
				case VK_EVENT_RESET : return "VK_EVENT_RESET";
				case VK_INCOMPLETE : return "VK_INCOMPLETE";
				case VK_ERROR_OUT_OF_HOST_MEMORY : return "VK_ERROR_OUT_OF_HOST_MEMORY";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY : return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
				case VK_ERROR_INITIALIZATION_FAILED : return "VK_ERROR_INITIALIZATION_FAILED";
				case VK_ERROR_DEVICE_LOST : return "VK_ERROR_DEVICE_LOST";
				case VK_ERROR_MEMORY_MAP_FAILED : return "VK_ERROR_MEMORY_MAP_FAILED";
				case VK_ERROR_LAYER_NOT_PRESENT : return "VK_ERROR_LAYER_NOT_PRESENT";
				case VK_ERROR_EXTENSION_NOT_PRESENT : return "VK_ERROR_EXTENSION_NOT_PRESENT";
				case VK_ERROR_FEATURE_NOT_PRESENT : return "VK_ERROR_FEATURE_NOT_PRESENT";
				case VK_ERROR_INCOMPATIBLE_DRIVER : return "VK_ERROR_INCOMPATIBLE_DRIVER";
				case VK_ERROR_TOO_MANY_OBJECTS : return "VK_ERROR_TOO_MANY_OBJECTS";
				case VK_ERROR_FORMAT_NOT_SUPPORTED : return "VK_ERROR_FORMAT_NOT_SUPPORTED";
				case VK_ERROR_FRAGMENTED_POOL : return "VK_ERROR_FRAGMENTED_POOL";
				case VK_ERROR_UNKNOWN : return "VK_ERROR_UNKNOWN";
				case VK_ERROR_OUT_OF_POOL_MEMORY : return "VK_ERROR_OUT_OF_POOL_MEMORY";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE : return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
				case VK_ERROR_FRAGMENTATION : return "VK_ERROR_FRAGMENTATION";
				case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS : return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
				case VK_PIPELINE_COMPILE_REQUIRED : return "VK_PIPELINE_COMPILE_REQUIRED";
				case VK_ERROR_SURFACE_LOST_KHR : return "VK_ERROR_SURFACE_LOST_KHR";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR : return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				case VK_SUBOPTIMAL_KHR : return "VK_SUBOPTIMAL_KHR";
				case VK_ERROR_OUT_OF_DATE_KHR : return "VK_ERROR_OUT_OF_DATE_KHR";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR : return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				case VK_ERROR_VALIDATION_FAILED_EXT : return "VK_ERROR_VALIDATION_FAILED_EXT";
				case VK_ERROR_INVALID_SHADER_NV : return "VK_ERROR_INVALID_SHADER_NV";
				case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR : return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
				case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR : return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
				case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR : return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
				case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR : return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
				case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR : return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
				case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR : return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
				case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT : return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
				case VK_ERROR_NOT_PERMITTED_KHR : return "VK_ERROR_NOT_PERMITTED_KHR";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT : return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				case VK_THREAD_IDLE_KHR : return "VK_THREAD_IDLE_KHR";
				case VK_THREAD_DONE_KHR : return "VK_THREAD_DONE_KHR";
				case VK_OPERATION_DEFERRED_KHR : return "VK_OPERATION_DEFERRED_KHR";
				case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
				case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
				case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
				defualt : return "UNKNOWN";
			}
			return "UNKNOWN";
		}
	};
};