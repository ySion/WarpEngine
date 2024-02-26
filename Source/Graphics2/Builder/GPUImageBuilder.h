#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUImage.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUImage> {
		public:
			using target_type = GPUImage;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;

				VkImageCreateInfo ci_image{
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = VK_FORMAT_R8G8B8A8_SRGB,
					.extent = {512,512,1},
					.mipLevels = 1,
					.arrayLayers = 1,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.tiling = VK_IMAGE_TILING_OPTIMAL,
					.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
				};

				VmaAllocationCreateInfo ci_allocation{
					.flags = 0,
					.usage = VMA_MEMORY_USAGE_AUTO,
					.requiredFlags = 0,
					.preferredFlags = 0,
					.memoryTypeBits = 0,
					.pool = VK_NULL_HANDLE,
					.pUserData = nullptr,
					.priority = 0.0f,
				};

				VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_image_config(
				VkFormat format,
				VkImageAspectFlags view_aspect_mask,
				VkExtent3D extent,
				VkImageType type,
				VkImageUsageFlagBits usage,
				VmaMemoryUsage memory_usage,
				VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
				uint32_t mip_levels = 1,
				uint32_t array_layers = 1,
				VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
				VkImageCreateFlags create_flags = 0
			) {
				create_info.ci_image.format = format;
				create_info.ci_image.extent = extent;
				create_info.ci_image.imageType = type;
				create_info.ci_image.usage = usage;
				create_info.ci_image.samples = samples;
				create_info.ci_image.mipLevels = mip_levels;
				create_info.ci_image.arrayLayers = array_layers;
				create_info.ci_image.tiling = tiling;
				create_info.ci_image.flags = create_flags;
				create_info.ci_allocation.usage = memory_usage;

				create_info.aspect_mask = view_aspect_mask;

				if (view_aspect_mask == 0) {
					if (is_depth_stencil_format(format)) {
						create_info.aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
					} else {
						create_info.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
					}
				}

				if (create_info.ci_image.usage == 0) {
					if (is_depth_stencil_format(format)) {
						create_info.ci_image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
					} else {
						create_info.ci_image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
					}
				}
				return *this;
			}

			self_type& set_image_init_layout(VkImageLayout init_layout) {
				create_info.ci_image.initialLayout = init_layout;
				return *this;
			}

			self_type& set_memory_config(VmaAllocationCreateFlags flags) {
				create_info.ci_allocation.flags = flags;
				return *this;
			}

			self_type& template_color_2D(VkExtent2D extent, VkFormat format, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, VkImageUsageFlags addtion_usages = 0) {
				create_info.ci_image = {
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = format,
					.extent = {extent.width, extent.height, 1},
					.mipLevels = 1,
					.arrayLayers = 1,
					.samples = sample_count,
					.tiling = VK_IMAGE_TILING_OPTIMAL,
					.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | addtion_usages,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
				};

				create_info.ci_image.usage |= addtion_usages;

				create_info.ci_allocation = {
					.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
				};

				create_info.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

				return *this;
			}

			self_type& template_depth32_stencil_u8_2D(VkExtent2D extent, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, VkImageUsageFlags addtion_usages = 0) {
				create_info.ci_image = {
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = VK_FORMAT_D32_SFLOAT_S8_UINT,
					.extent = {extent.width, extent.height, 1},
					.mipLevels = 1,
					.arrayLayers = 1,
					.samples = sample_count,
					.tiling = VK_IMAGE_TILING_OPTIMAL,
					.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | addtion_usages,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
				};

				create_info.ci_allocation = {
					.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
				};

				create_info.aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				return *this;
			}

			target_type* make(bool replace = false) {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkImage image{};
					VmaAllocation allocation{};
					if (res = vmaCreateImage(GPUFactory::get_vma(), &create_info.ci_image, &create_info.ci_allocation, &image, &allocation, nullptr);
						VK_SUCCESS != res || !image || !allocation) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_image = image;
					obj->m_extent = create_info.ci_image.extent;
					obj->m_image_format = create_info.ci_image.format;
					obj->m_image_type = create_info.ci_image.imageType;
					obj->m_sample = create_info.ci_image.samples;
					obj->m_tiling = create_info.ci_image.tiling;
					obj->m_usage = create_info.ci_image.usage;
					obj->m_mip_count = create_info.ci_image.mipLevels;
					obj->m_layer_count = create_info.ci_image.arrayLayers;
					obj->m_memory = allocation;
					obj->m_memory_usage = create_info.ci_allocation.usage;
					obj->m_memory_flag = create_info.ci_allocation.flags;
					obj->m_born_init_layout = create_info.ci_image.initialLayout;
					
					VkImageViewCreateInfo ci_view = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
							.pNext = nullptr,
							.flags = 0,
							.image = image,
							.viewType = static_cast<VkImageViewType>(create_info.ci_image.imageType),
							.format = create_info.ci_image.format,
							.components = {
								.r = VK_COMPONENT_SWIZZLE_R,
								.g = VK_COMPONENT_SWIZZLE_G,
								.b = VK_COMPONENT_SWIZZLE_B,
								.a = VK_COMPONENT_SWIZZLE_A
							},
							.subresourceRange = {
								.aspectMask = create_info.aspect_mask,
								.baseMipLevel = 0,
								.levelCount = create_info.ci_image.mipLevels,
								.baseArrayLayer = 0,
								.layerCount = create_info.ci_image.arrayLayers
							}
					};

					obj->add_view(ci_view);

					m_manager->add(obj);
					return obj;

				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

			MVector<target_type*> make_multi(uint32_t count, bool replace = false) {
				MVector<target_type*> result;
				MString name = create_info.name;
				for (uint32_t i = 0; i < count; ++i) {
					if (!create_info.name.empty()) {
						create_info.name = to_MString(std::format("{}_{}", name, i));
					}
					if (auto obj = make(replace)) {
						result.push_back(obj);
					}
				}
				return result;
			}

			MVector<target_type*> make_multi(const MVector<MString>& names, bool replace = false) {
				MVector<target_type*> result;
				for (uint32_t i = 0; i < names.size(); ++i) {
					create_info.name = names[i];
					auto obj = make(replace);
					if (obj) {
						result.push_back(obj);
					}
				}
				return result;
			}

		private:
			CreateInfo create_info{};
			GPUResourceManager<target_type>* m_manager{};
		};
	}
}