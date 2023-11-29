#pragma once

#include "Graphics/GPUResource.h"
#include "Graphics/GPUResource/GPUImage.h"
#include "Graphics/GPUResourceHandle.h"

#include "Render/RTGTaskNodes/TaskNodeBase.h"

namespace Warp::Render {

	struct RenderTaskResourceTypeMapper
	{
		static const inline MMap<MString, VkFormat> type_mapper {
			{"format_r4g4_unorm_pack8", VK_FORMAT_R4G4_UNORM_PACK8},
			{"format_r4g4b4a4_unorm_pack16", VK_FORMAT_R4G4B4A4_UNORM_PACK16},
			{"format_b4g4r4a4_unorm_pack16", VK_FORMAT_B4G4R4A4_UNORM_PACK16},
			{"format_r5g6b5_unorm_pack16", VK_FORMAT_R5G6B5_UNORM_PACK16},
			{"format_b5g6r5_unorm_pack16", VK_FORMAT_B5G6R5_UNORM_PACK16},
			{"format_r5g5b5a1_unorm_pack16", VK_FORMAT_R5G5B5A1_UNORM_PACK16},
			{"format_b5g5r5a1_unorm_pack16", VK_FORMAT_B5G5R5A1_UNORM_PACK16},
			{"format_a1r5g5b5_unorm_pack16", VK_FORMAT_A1R5G5B5_UNORM_PACK16},
			{"format_r8_unorm", VK_FORMAT_R8_UNORM},
			{"format_r8_snorm", VK_FORMAT_R8_SNORM},
			{"format_r8_uscaled", VK_FORMAT_R8_USCALED},
			{"format_r8_sscaled", VK_FORMAT_R8_SSCALED},
			{"format_r8_uint", VK_FORMAT_R8_UINT},
			{"format_r8_sint", VK_FORMAT_R8_SINT},
			{"format_r8_srgb", VK_FORMAT_R8_SRGB},
			{"format_r8g8_unorm", VK_FORMAT_R8G8_UNORM},
			{"format_r8g8_snorm", VK_FORMAT_R8G8_SNORM},
			{"format_r8g8_uscaled", VK_FORMAT_R8G8_USCALED},
			{"format_r8g8_sscaled", VK_FORMAT_R8G8_SSCALED},
			{"format_r8g8_uint", VK_FORMAT_R8G8_UINT},
			{"format_r8g8_sint", VK_FORMAT_R8G8_SINT},
			{"format_r8g8_srgb", VK_FORMAT_R8G8_SRGB},
			{"format_r8g8b8_unorm", VK_FORMAT_R8G8B8_UNORM},
			{"format_r8g8b8_snorm", VK_FORMAT_R8G8B8_SNORM},
			{"format_r8g8b8_uscaled", VK_FORMAT_R8G8B8_USCALED},
			{"format_r8g8b8_sscaled", VK_FORMAT_R8G8B8_SSCALED},
			{"format_r8g8b8_uint", VK_FORMAT_R8G8B8_UINT},
			{"format_r8g8b8_sint", VK_FORMAT_R8G8B8_SINT},
			{"format_r8g8b8_srgb", VK_FORMAT_R8G8B8_SRGB},
			{"format_b8g8r8_unorm", VK_FORMAT_B8G8R8_UNORM},
			{"format_b8g8r8_snorm", VK_FORMAT_B8G8R8_SNORM},
			{"format_b8g8r8_uscaled", VK_FORMAT_B8G8R8_USCALED},
			{"format_b8g8r8_sscaled", VK_FORMAT_B8G8R8_SSCALED},
			{"format_b8g8r8_uint", VK_FORMAT_B8G8R8_UINT},
			{"format_b8g8r8_sint", VK_FORMAT_B8G8R8_SINT},
			{"format_b8g8r8_srgb", VK_FORMAT_B8G8R8_SRGB},
			{"format_r8g8b8a8_unorm", VK_FORMAT_R8G8B8A8_UNORM},
			{"format_r8g8b8a8_snorm", VK_FORMAT_R8G8B8A8_SNORM},
			{"format_r8g8b8a8_uscaled", VK_FORMAT_R8G8B8A8_USCALED},
			{"format_r8g8b8a8_sscaled", VK_FORMAT_R8G8B8A8_SSCALED},
			{"format_r8g8b8a8_uint", VK_FORMAT_R8G8B8A8_UINT},
			{"format_r8g8b8a8_sint", VK_FORMAT_R8G8B8A8_SINT},
			{"format_r8g8b8a8_srgb", VK_FORMAT_R8G8B8A8_SRGB},
			{"format_b8g8r8a8_unorm", VK_FORMAT_B8G8R8A8_UNORM},
			{"format_b8g8r8a8_snorm", VK_FORMAT_B8G8R8A8_SNORM},
			{"format_b8g8r8a8_uscaled", VK_FORMAT_B8G8R8A8_USCALED},
			{"format_b8g8r8a8_sscaled", VK_FORMAT_B8G8R8A8_SSCALED},
			{"format_b8g8r8a8_uint", VK_FORMAT_B8G8R8A8_UINT},
			{"format_b8g8r8a8_sint", VK_FORMAT_B8G8R8A8_SINT},
			{"format_b8g8r8a8_srgb", VK_FORMAT_B8G8R8A8_SRGB},
			{"format_a8b8g8r8_unorm_pack32", VK_FORMAT_A8B8G8R8_UNORM_PACK32},
			{"format_a8b8g8r8_snorm_pack32", VK_FORMAT_A8B8G8R8_SNORM_PACK32},
			{"format_a8b8g8r8_uscaled_pack32", VK_FORMAT_A8B8G8R8_USCALED_PACK32},
			{"format_a8b8g8r8_sscaled_pack32", VK_FORMAT_A8B8G8R8_SSCALED_PACK32},
			{"format_a8b8g8r8_uint_pack32", VK_FORMAT_A8B8G8R8_UINT_PACK32},
			{"format_a8b8g8r8_sint_pack32", VK_FORMAT_A8B8G8R8_SINT_PACK32},
			{"format_a8b8g8r8_srgb_pack32", VK_FORMAT_A8B8G8R8_SRGB_PACK32},
			{"format_a2r10g10b10_unorm_pack32", VK_FORMAT_A2R10G10B10_UNORM_PACK32},
			{"format_a2r10g10b10_snorm_pack32", VK_FORMAT_A2R10G10B10_SNORM_PACK32},
			{"format_a2r10g10b10_uscaled_pack32", VK_FORMAT_A2R10G10B10_USCALED_PACK32},
			{"format_a2r10g10b10_sscaled_pack32", VK_FORMAT_A2R10G10B10_SSCALED_PACK32},
			{"format_a2r10g10b10_uint_pack32", VK_FORMAT_A2R10G10B10_UINT_PACK32},
			{"format_a2r10g10b10_sint_pack32", VK_FORMAT_A2R10G10B10_SINT_PACK32},
			{"format_a2b10g10r10_unorm_pack32", VK_FORMAT_A2B10G10R10_UNORM_PACK32},
			{"format_a2b10g10r10_snorm_pack32", VK_FORMAT_A2B10G10R10_SNORM_PACK32},
			{"format_a2b10g10r10_uscaled_pack32", VK_FORMAT_A2B10G10R10_USCALED_PACK32},
			{"format_a2b10g10r10_sscaled_pack32", VK_FORMAT_A2B10G10R10_SSCALED_PACK32},
			{"format_a2b10g10r10_uint_pack32", VK_FORMAT_A2B10G10R10_UINT_PACK32},
			{"format_a2b10g10r10_sint_pack32", VK_FORMAT_A2B10G10R10_SINT_PACK32},
			{"format_r16_unorm", VK_FORMAT_R16_UNORM},
			{"format_r16_snorm", VK_FORMAT_R16_SNORM},
			{"format_r16_uscaled", VK_FORMAT_R16_USCALED},
			{"format_r16_sscaled", VK_FORMAT_R16_SSCALED},
			{"format_r16_uint", VK_FORMAT_R16_UINT},
			{"format_r16_sint", VK_FORMAT_R16_SINT},
			{"format_r16_sfloat", VK_FORMAT_R16_SFLOAT},
			{"format_r16g16_unorm", VK_FORMAT_R16G16_UNORM},
			{"format_r16g16_snorm", VK_FORMAT_R16G16_SNORM},
			{"format_r16g16_uscaled", VK_FORMAT_R16G16_USCALED},
			{"format_r16g16_sscaled", VK_FORMAT_R16G16_SSCALED},
			{"format_r16g16_uint", VK_FORMAT_R16G16_UINT},
			{"format_r16g16_sint", VK_FORMAT_R16G16_SINT},
			{"format_r16g16_sfloat", VK_FORMAT_R16G16_SFLOAT},
			{"format_r16g16b16_unorm", VK_FORMAT_R16G16B16_UNORM},
			{"format_r16g16b16_snorm", VK_FORMAT_R16G16B16_SNORM},
			{"format_r16g16b16_uscaled", VK_FORMAT_R16G16B16_USCALED},
			{"format_r16g16b16_sscaled", VK_FORMAT_R16G16B16_SSCALED},
			{"format_r16g16b16_uint", VK_FORMAT_R16G16B16_UINT},
			{"format_r16g16b16_sint", VK_FORMAT_R16G16B16_SINT},
			{"format_r16g16b16_sfloat", VK_FORMAT_R16G16B16_SFLOAT},
			{"format_r16g16b16a16_unorm", VK_FORMAT_R16G16B16A16_UNORM},
			{"format_r16g16b16a16_snorm", VK_FORMAT_R16G16B16A16_SNORM},
			{"format_r16g16b16a16_uscaled", VK_FORMAT_R16G16B16A16_USCALED},
			{"format_r16g16b16a16_sscaled", VK_FORMAT_R16G16B16A16_SSCALED},
			{"format_r16g16b16a16_uint", VK_FORMAT_R16G16B16A16_UINT},
			{"format_r16g16b16a16_sint", VK_FORMAT_R16G16B16A16_SINT},
			{"format_r16g16b16a16_sfloat", VK_FORMAT_R16G16B16A16_SFLOAT},
			{"format_r32_uint", VK_FORMAT_R32_UINT},
			{"format_r32_sint", VK_FORMAT_R32_SINT},
			{"format_r32_sfloat", VK_FORMAT_R32_SFLOAT},
			{"format_r32g32_uint", VK_FORMAT_R32G32_UINT},
			{"format_r32g32_sint", VK_FORMAT_R32G32_SINT},
			{"format_r32g32_sfloat", VK_FORMAT_R32G32_SFLOAT},
			{"format_r32g32b32_uint", VK_FORMAT_R32G32B32_UINT},
			{"format_r32g32b32_sint", VK_FORMAT_R32G32B32_SINT},
			{"format_r32g32b32_sfloat", VK_FORMAT_R32G32B32_SFLOAT},
			{"format_r32g32b32a32_uint", VK_FORMAT_R32G32B32A32_UINT},
			{"format_r32g32b32a32_sint", VK_FORMAT_R32G32B32A32_SINT},
			{"format_r32g32b32a32_sfloat", VK_FORMAT_R32G32B32A32_SFLOAT},
			{"format_r64_uint", VK_FORMAT_R64_UINT},
			{"format_r64_sint", VK_FORMAT_R64_SINT},
			{"format_r64_sfloat", VK_FORMAT_R64_SFLOAT},
			{"format_r64g64_uint", VK_FORMAT_R64G64_UINT},
			{"format_r64g64_sint", VK_FORMAT_R64G64_SINT},
			{"format_r64g64_sfloat", VK_FORMAT_R64G64_SFLOAT},
			{"format_r64g64b64_uint", VK_FORMAT_R64G64B64_UINT},
			{"format_r64g64b64_sint", VK_FORMAT_R64G64B64_SINT},
			{"format_r64g64b64_sfloat", VK_FORMAT_R64G64B64_SFLOAT},
			{"format_r64g64b64a64_uint", VK_FORMAT_R64G64B64A64_UINT},
			{"format_r64g64b64a64_sint", VK_FORMAT_R64G64B64A64_SINT},
			{"format_r64g64b64a64_sfloat", VK_FORMAT_R64G64B64A64_SFLOAT},
			{"format_b10g11r11_ufloat_pack32", VK_FORMAT_B10G11R11_UFLOAT_PACK32},
			{"format_e5b9g9r9_ufloat_pack32", VK_FORMAT_E5B9G9R9_UFLOAT_PACK32},
			{"format_d16_unorm", VK_FORMAT_D16_UNORM},
			{"format_x8_d24_unorm_pack32", VK_FORMAT_X8_D24_UNORM_PACK32},
			{"format_d32_sfloat", VK_FORMAT_D32_SFLOAT},
			{"format_s8_uint", VK_FORMAT_S8_UINT},
			{"format_d16_unorm_s8_uint", VK_FORMAT_D16_UNORM_S8_UINT},
			{"format_d24_unorm_s8_uint", VK_FORMAT_D24_UNORM_S8_UINT},
			{"format_d32_sfloat_s8_uint", VK_FORMAT_D32_SFLOAT_S8_UINT},
			{"format_bc1_rgb_unorm_block", VK_FORMAT_BC1_RGB_UNORM_BLOCK},
			{"format_bc1_rgb_srgb_block", VK_FORMAT_BC1_RGB_SRGB_BLOCK},
			{"format_bc1_rgba_unorm_block", VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
			{"format_bc1_rgba_srgb_block", VK_FORMAT_BC1_RGBA_SRGB_BLOCK},
			{"format_bc2_unorm_block", VK_FORMAT_BC2_UNORM_BLOCK},
			{"format_bc2_srgb_block", VK_FORMAT_BC2_SRGB_BLOCK},
			{"format_bc3_unorm_block", VK_FORMAT_BC3_UNORM_BLOCK},
			{"format_bc3_srgb_block", VK_FORMAT_BC3_SRGB_BLOCK},
			{"format_bc4_unorm_block", VK_FORMAT_BC4_UNORM_BLOCK},
			{"format_bc4_snorm_block", VK_FORMAT_BC4_SNORM_BLOCK},
			{"format_bc5_unorm_block", VK_FORMAT_BC5_UNORM_BLOCK},
			{"format_bc5_snorm_block", VK_FORMAT_BC5_SNORM_BLOCK},
			{"format_bc6h_ufloat_block", VK_FORMAT_BC6H_UFLOAT_BLOCK},
			{"format_bc6h_sfloat_block", VK_FORMAT_BC6H_SFLOAT_BLOCK},
			{"format_bc7_unorm_block", VK_FORMAT_BC7_UNORM_BLOCK},
			{"format_bc7_srgb_block", VK_FORMAT_BC7_SRGB_BLOCK},
			{"format_etc2_r8g8b8_unorm_block", VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK},
			{"format_etc2_r8g8b8_srgb_block", VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK},
			{"format_etc2_r8g8b8a1_unorm_block", VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK},
			{"format_etc2_r8g8b8a1_srgb_block", VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK},
			{"format_etc2_r8g8b8a8_unorm_block", VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK},
			{"format_etc2_r8g8b8a8_srgb_block", VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK},
			{"format_eac_r11_unorm_block", VK_FORMAT_EAC_R11_UNORM_BLOCK},
			{"format_eac_r11_snorm_block", VK_FORMAT_EAC_R11_SNORM_BLOCK},
			{"format_eac_r11g11_unorm_block", VK_FORMAT_EAC_R11G11_UNORM_BLOCK},
			{"format_eac_r11g11_snorm_block", VK_FORMAT_EAC_R11G11_SNORM_BLOCK},
			{"format_g8b8g8r8_422_unorm", VK_FORMAT_G8B8G8R8_422_UNORM},
			{"format_b8g8r8g8_422_unorm", VK_FORMAT_B8G8R8G8_422_UNORM},
			{"format_g8_b8_r8_3plane_420_unorm", VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM},
			{"format_g8_b8r8_2plane_420_unorm", VK_FORMAT_G8_B8R8_2PLANE_420_UNORM},
			{"format_g8_b8_r8_3plane_422_unorm", VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM},
			{"format_g8_b8r8_2plane_422_unorm", VK_FORMAT_G8_B8R8_2PLANE_422_UNORM},
			{"format_g8_b8_r8_3plane_444_unorm", VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM},
			{"format_r10x6_unorm_pack16", VK_FORMAT_R10X6_UNORM_PACK16},
			{"format_r10x6g10x6_unorm_2pack16", VK_FORMAT_R10X6G10X6_UNORM_2PACK16},
			{"format_r10x6g10x6b10x6a10x6_unorm_4pack16", VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16},
			{"format_g10x6b10x6g10x6r10x6_422_unorm_4pack16", VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16},
			{"format_b10x6g10x6r10x6g10x6_422_unorm_4pack16", VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16},
			{"format_g10x6_b10x6_r10x6_3plane_420_unorm_3pack16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16},
			{"format_g10x6_b10x6r10x6_2plane_420_unorm_3pack16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16},
			{"format_g10x6_b10x6_r10x6_3plane_422_unorm_3pack16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16},
			{"format_g10x6_b10x6r10x6_2plane_422_unorm_3pack16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16},
			{"format_g10x6_b10x6_r10x6_3plane_444_unorm_3pack16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16},
			{"format_r12x4_unorm_pack16", VK_FORMAT_R12X4_UNORM_PACK16},
			{"format_r12x4g12x4_unorm_2pack16", VK_FORMAT_R12X4G12X4_UNORM_2PACK16},
			{"format_r12x4g12x4b12x4a12x4_unorm_4pack16", VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16},
			{"format_g12x4b12x4g12x4r12x4_422_unorm_4pack16", VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16},
			{"format_b12x4g12x4r12x4g12x4_422_unorm_4pack16", VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16},
			{"format_g12x4_b12x4_r12x4_3plane_420_unorm_3pack16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16},
			{"format_g12x4_b12x4r12x4_2plane_420_unorm_3pack16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16},
			{"format_g12x4_b12x4_r12x4_3plane_422_unorm_3pack16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16},
			{"format_g12x4_b12x4r12x4_2plane_422_unorm_3pack16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16},
			{"format_g12x4_b12x4_r12x4_3plane_444_unorm_3pack16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16},
			{"format_g16b16g16r16_422_unorm", VK_FORMAT_G16B16G16R16_422_UNORM},
			{"format_b16g16r16g16_422_unorm", VK_FORMAT_B16G16R16G16_422_UNORM},
			{"format_g16_b16_r16_3plane_420_unorm", VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM},
			{"format_g16_b16r16_2plane_420_unorm", VK_FORMAT_G16_B16R16_2PLANE_420_UNORM},
			{"format_g16_b16_r16_3plane_422_unorm", VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM},
			{"format_g16_b16r16_2plane_422_unorm", VK_FORMAT_G16_B16R16_2PLANE_422_UNORM},
			{"format_g16_b16_r16_3plane_444_unorm", VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM},
			{"format_g8_b8r8_2plane_444_unorm", VK_FORMAT_G8_B8R8_2PLANE_444_UNORM},
			{"format_g10x6_b10x6r10x6_2plane_444_unorm_3pack16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16},
			{"format_g12x4_b12x4r12x4_2plane_444_unorm_3pack16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16},
			{"format_g16_b16r16_2plane_444_unorm", VK_FORMAT_G16_B16R16_2PLANE_444_UNORM},
			{"format_a4r4g4b4_unorm_pack16", VK_FORMAT_A4R4G4B4_UNORM_PACK16},
			{"format_a4b4g4r4_unorm_pack16", VK_FORMAT_A4B4G4R4_UNORM_PACK16},
			{"format_r16g16_s10_5_nv", VK_FORMAT_R16G16_S10_5_NV},
			{"format_a1b5g5r5_unorm_pack16_khr", VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR},
			{"format_a8_unorm_khr", VK_FORMAT_A8_UNORM_KHR}
		};
	};

	struct RenderTaskResourceImageNodeCreateInfo {
		MString name;
		MString type;
	};

	struct RenderTaskResourceBufferNodeCreateInfo {
		MString name;
		MString type;
		uint64_t size;
	};

	struct RenderTaskTaskNodeCreateInfo {
		MString name;
		MString type;
		MVector<uint32_t> ref_images;
		MVector<uint32_t> ref_buffers;
		MVector<uint32_t> ref_previous_frame_images;
		MVector<uint32_t> ref_previous_frame_buffers;
	};

	class RenderSystem;

	class RenderTaskGraph {

		friend class RenderTaskGraphDomain;

		RenderTaskGraph(const MString& name, RenderSystem* render_system, VkExtent2D static_resolution, uint32_t frame_idx);
	public:

		~RenderTaskGraph() = default;

		RenderTaskGraph(const RenderTaskGraph&) = delete;

		RenderTaskGraph(RenderTaskGraph&&) = delete;

		RenderTaskGraph& operator=(const RenderTaskGraph&) = delete;

		RenderTaskGraph& operator=(RenderTaskGraph&&) = delete;

		//image_ for format,
		//mask_ for mask,
		//mip_ for mip count,
		//layer_ for layer count,
		//sample_ for sample count,
		//extentz_ for extentz,
		//type_ for image type,
		//usage_ for usage
		RenderTaskGraph& add_image_node(const MString& name, const MString& type) {

			if (m_image_name_mapper.contains(name)) {
				LOGE("[RenderTaskGraph] \"{}\" try to add Image node \"{}\", but it's already exists", m_name, name);
				return *this;
			}

			m_image_descs.emplace_back(name, type);
			m_image_name_mapper.insert({ name, static_cast<uint32_t>(m_image_descs.size() - 1) });

			return *this;
		}

		RenderTaskGraph& add_buffer_node(const MString& name, const MString& type, uint64_t size) {

			if (m_buffer_name_mapper.contains(name)) {
				LOGE("[RenderTaskGraph] \"{}\" try to add Buffer node \"{}\", but it's already exists", m_name, name);
				return *this;
			}

			m_buffer_descs.emplace_back(name, type, size);
			m_buffer_name_mapper.insert({ name, static_cast<uint32_t>(m_image_descs.size() - 1) });

			return *this;
		}

		RenderTaskGraph& add_task_node(const MString& name, const MString& type, 
			const MVector<uint32_t>& ref_images, 
			const MVector<uint32_t>& ref_buffers = {},
			const MVector<uint32_t>& ref_previous_frame_images = {},
			const MVector<uint32_t>& ref_previous_frame_buffers = {}
		) {
			if (m_task_name_mapper.contains(name)) {
				LOGE("[RenderTaskGraph] \"{}\" try to add Task node \"{}\", but it's already exists", m_name, name);
				return *this;
			}

			if (!RenderTaTaskNodeNameMapper::name_2_task_node.contains(type)) {
				LOGE("[RenderTaskGraph] Task node type \"{}\" not found", type);
				return *this;
			}

			const auto current_image_count = static_cast<uint32_t>(m_image_descs.size());
			for (unsigned int i : ref_images) {
				if (current_image_count <= i) {
					LOGE("[RenderTaskGraph] \"{}\", Image Ref index \"{}\" is larger than image count \"{}\".", m_name, i);
					return *this;
				}
			}
			for (unsigned int i : ref_previous_frame_images) {
				if (current_image_count <= i) {
					LOGE("[RenderTaskGraph] \"{}\", Previous Image Ref index \"{}\" is larger than image count \"{}\".", m_name, current_image_count, i);
					return *this;
				}
			}

			const auto current_buffer_count = static_cast<uint32_t>(m_buffer_descs.size());
			for (unsigned int i : ref_buffers) {
				if (current_buffer_count <= i) {
					LOGE("[RenderTaskGraph] \"{}\", Buffer Ref index \"{}\" is larger than buffer count \"{}\".", m_name, i);
					return *this;
				}
			}

			for (unsigned int i : ref_previous_frame_buffers) {
				if (current_buffer_count <= i) {
					LOGE("[RenderTaskGraph] \"{}\", Previous Buffer Ref index \"{}\" is larger than buffer count \"{}\".", m_name, current_buffer_count, i);
					return *this;
				}
			}

			m_task_descs.emplace_back(name, type, ref_images, ref_buffers, ref_previous_frame_images, ref_previous_frame_buffers);
			m_task_name_mapper.insert({ name, static_cast<uint32_t>(m_task_descs.size() - 1)});

			return *this;
		}

		RenderTaskGraph& set_screen_resolution(const VkExtent2D& resolution) {
			m_resource_static_resolution = resolution;
			return *this;
		}

		void clear() {
			m_image_descs.clear();
			m_buffer_descs.clear();
			m_task_descs.clear();

			m_image_name_mapper.clear();
			m_buffer_name_mapper.clear();
			m_task_name_mapper.clear();
		}

		bool compile() {

			m_images.clear();
			m_buffers.clear();
			m_task_nodes_handles.clear();

			for(auto& r : m_image_descs) {
				if(!make_image_node(r)) {
					clear();
					return false;
				}
			}

			for (auto& r : m_buffer_descs) {
				if (!make_buffer_node(r)) {
					clear();
					return false;
				}
			}

			for(auto& t : m_task_descs) {
				if(!make_task_node(t)) {
					clear();
					return false;
				}
			}

			return true;
		}


		inline VkExtent2D get_static_resolution() const {
			return m_resource_static_resolution;
		}

		inline RenderSystem* get_render_system() const {
			return m_render_system;
		}

		inline void generate_commands(GPU::GPUCommandBuffer* commandbuf)
		{
			for (auto& t : m_task_nodes_handles) {
				t->generate_commands(commandbuf);
			}
		}

	private:
		bool make_task_node(const RenderTaskTaskNodeCreateInfo& node);

		bool make_buffer_node(const RenderTaskResourceBufferNodeCreateInfo& node);

		bool make_image_node(const RenderTaskResourceImageNodeCreateInfo& node);

		inline void set_previous_frame_graph(RenderTaskGraph* graph) {
			m_previous_frame_graph = graph;
		}

	private:
		MString m_name;
		uint32_t m_frame_idx = {};
		RenderSystem* m_render_system{ nullptr };

		//screen resolution
		VkExtent2D m_resource_static_resolution{ 0, 0 };
		//VkExtent2D m_dynamic_resolution{0, 0};

		//Descs
		MVector<RenderTaskResourceImageNodeCreateInfo> m_image_descs{};
		MVector<RenderTaskResourceBufferNodeCreateInfo> m_buffer_descs{};
		MVector<RenderTaskTaskNodeCreateInfo> m_task_descs{};

		MMap<MString, uint32_t> m_image_name_mapper{};
		MMap<MString, uint32_t> m_buffer_name_mapper{};
		MMap<MString, uint32_t> m_task_name_mapper{};

		//compiled data
		MVector<GPU::GPUResourceHandle<GPU::GPUImage>> m_images{};
		MVector<GPU::GPUResourceHandle<GPU::GPUBuffer>> m_buffers{};

		MVector<std::unique_ptr<TaskNodeBase>> m_task_nodes_handles{};

		RenderTaskGraph* m_previous_frame_graph{ nullptr };
	};
}
