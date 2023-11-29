#include "RenderTaskGraph.h"
#include "RenderSystem.h"

Warp::Render::RenderTaskGraph::RenderTaskGraph(const MString& name, RenderSystem* render_system, VkExtent2D static_resolution, uint32_t frame_idx) :m_name(name), m_frame_idx(frame_idx), m_render_system(render_system), m_resource_static_resolution(static_resolution) {

}

bool Warp::Render::RenderTaskGraph::make_task_node(const RenderTaskTaskNodeCreateInfo& node) {

	MVector<GPU::GPUImage*> images{ node.ref_images.size() };
	std::ranges::transform(node.ref_images, images.begin(), [&](uint32_t index) {
		return m_images[index].get();
	});

	MVector<GPU::GPUImage*> prev_images{ node.ref_previous_frame_images.size() };
	std::ranges::transform(node.ref_previous_frame_images, prev_images.begin(), [&](uint32_t index) {
		return m_previous_frame_graph->m_images[index].get();
	});

	images.insert(images.end(), prev_images.begin(), prev_images.end());

	MVector<GPU::GPUBuffer*> buffers{ node.ref_buffers.size() };
	std::ranges::transform(node.ref_buffers, buffers.begin(), [&](uint32_t index) {
		return m_buffers[index].get();
	});

	MVector<GPU::GPUBuffer*> prev_buffers{ node.ref_previous_frame_buffers.size() };
	std::ranges::transform(node.ref_previous_frame_buffers, prev_buffers.begin(), [&](uint32_t index) {
		return m_previous_frame_graph->m_buffers[index].get();
	});

	buffers.insert(buffers.end(), prev_buffers.begin(), prev_buffers.end());

	const auto ptr = RenderTaTaskNodeNameMapper::name_2_task_node.at(node.type)(this, images, buffers);
	if (!ptr) {
		LOGE("[RenderTaskGraph]\"{}\" try to make Task Node \"{}\", unknown error happeds.", m_name, node.type);
		return false;
	}

	if (!ptr->prepare()) {
		LOGE("[RenderTaskGraph]\"{}\" try to make Task Node \"{}\", prepare failed.", m_name, node.type);
		return false;
	}

	return true;
}

bool Warp::Render::RenderTaskGraph::make_buffer_node(const RenderTaskResourceBufferNodeCreateInfo& node) {

	const MString format_name = mformat("{}_{}", m_name, node.name);
	auto builder = m_render_system->m_render_resource_group->get_buffer_manager()->builder(format_name);

	if (node.type == "uniform") {
		builder.template_uniform_buffer(node.size);
	} else if (node.type == "gpu_vertex") {
		builder.template_on_device_vertex_copy_dst(node.size);
	} else if (node.type == "gpu_index") {
		builder.template_on_device_vertex_index_copy_dst(node.size);
	} else if (node.type == "upload") {
		builder.template_upload(node.size);
	} else if (node.type == "readback") {
		builder.template_readback(node.size);
	} else if (node.type == "gpu_storage") {
		builder.template_on_device_storage_copy_src_dst(node.size);
	} else if (node.type == "gpu_copy") {
		builder.template_gpu_copy_src_dst(node.size);
	} else {
		LOGE("[RenderTaskGraph]\"{}\" try to make Buffer, type \"{}\" not found.", m_name, node.type);
		return false;
	}

	auto ptr = builder.make(true);
	if (!ptr) return false;

	m_buffers.emplace_back(ptr);

	return true;
}

bool Warp::Render::RenderTaskGraph::make_image_node(const RenderTaskResourceImageNodeCreateInfo& node) {

	VkFormat format{};
	VkImageAspectFlags mask = 0;
	uint32_t mip_count = 1;
	uint32_t layer_count = 1;
	uint32_t sample_count = 1;
	uint32_t extentz = 1;
	VkImageType image_type = VK_IMAGE_TYPE_2D;
	auto res = mstring_split(node.type, ';');
	VkImageUsageFlags usage_flag = 0;
	for (auto& r : res) {
		if (r.starts_with("format_")) {
			if (RenderTaskResourceTypeMapper::type_mapper.contains(r)) {
				format = RenderTaskResourceTypeMapper::type_mapper.at(r);
			} else {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource image format \"{}\" not found.", m_name, r);
				return false;
			}
		} else if (r.starts_with("mask_")) {
			if (r == "mask_color") {
				mask = VK_IMAGE_ASPECT_COLOR_BIT;
			} else if (r == "mask_depth") {
				mask = VK_IMAGE_ASPECT_DEPTH_BIT;
			} else if (r == "mask_stencil") {
				mask = VK_IMAGE_ASPECT_STENCIL_BIT;
			} else if (r == "mask_depth_stencil") {
				mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			} else {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);
				return false;
			}
		} else if (r.starts_with("mip_")) {
			auto part = mstring_split(r, '_');
			if (part.size() != 2) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
			try {
				mip_count = std::stoi(part[1].c_str());
			} catch (...) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
		} else if (r.starts_with("layer_")) {
			auto part = mstring_split(r, '_');
			if (part.size() != 2) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
			try {
				layer_count = std::stoi(part[1].c_str());
			} catch (...) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
		} else if (r.starts_with("sample_")) {
			auto part = mstring_split(r, '_');
			if (part.size() != 2) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}

			try {
				sample_count = std::stoi(part[1].c_str());
			} catch (...) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
		} else if (r.starts_with("extentz_")) {
			auto part = mstring_split(r, '_');
			if (part.size() != 2) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}

			try {
				extentz = std::stoi(part[1].c_str());
			} catch (...) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);

				return false;
			}
		} else if (r.starts_with("type_")) {
			if (r == "1d") {
				image_type = VK_IMAGE_TYPE_1D;
			} else if (r == "2d") {
				image_type = VK_IMAGE_TYPE_2D;
			} else if (r == "3d") {
				image_type = VK_IMAGE_TYPE_3D;
			}
		} else if (r.starts_with("usage_")) {
			auto part = mstring_split(r, '_');
			if (part.size() < 2) {
				LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct", m_name, r);

				return false;
			} else {
				for (uint32_t idx = 1; idx < part.size(); ++idx) {
					if (part[idx] == "TS") {
						usage_flag |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
					} else if (part[idx] == "TD") {
						usage_flag |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
					} else if (part[idx] == "C") {
						usage_flag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
					} else if (part[idx] == "D") {
						usage_flag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
					} else if (part[idx] == "IA") {
						usage_flag |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
					} else if (part[idx] == "TA") {
						usage_flag |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
					} else if (part[idx] == "ST") {
						usage_flag |= VK_IMAGE_USAGE_STORAGE_BIT;
					} else if (part[idx] == "SA") {
						usage_flag |= VK_IMAGE_USAGE_SAMPLED_BIT;
					}
				}
			}
		} else if (r.empty()) {
			continue;
		} else {
			LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument \"{}\" is not correct.", m_name, r);
			return false;
		}
	}

	if (format == 0) {
		LOGE("[RenderTaskGraph]\"{}\" try to make Image, Resource make argument is empty.", m_name);
		return false;
	}

	const MString format_name = mformat("{}_{}", m_name, node.name);
	auto image_res = m_render_system->m_render_resource_group->get_image_manager()->builder(format_name)
		.set_image_config(format, mask, { m_resource_static_resolution.width, m_resource_static_resolution.height, extentz },
				  image_type, static_cast<VkImageUsageFlagBits>(usage_flag), VMA_MEMORY_USAGE_AUTO, static_cast<VkSampleCountFlagBits>(sample_count),
				  mip_count, layer_count).make(true);

	m_images.emplace_back(image_res);
	return true;
}