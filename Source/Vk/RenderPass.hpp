#pragma once

#include "VkBase.hpp"
#include "Device.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, RenderPass)

namespace Warp::Gpu {


	struct AttachmentDescription {
		VkFormat format;
		VkSampleCountFlagBits samples;
		VkAttachmentLoadOp load_op;
		VkAttachmentStoreOp store_op;
		VkAttachmentLoadOp stencil_load_op;
		VkAttachmentStoreOp stencil_store_op;
		VkImageLayout initial_layout;
		VkImageLayout final_layout;
		VkImageLayout ref_layout;
	};

	class RenderPass : public Inherit<RenderPass, Object> {
	public:
		RenderPass(Device* device);

		~RenderPass() override;

		inline Device* device() const { return _device; }

		VkRenderPass vk() const { return _render_pass; }

		operator VkRenderPass() const { return _render_pass; }

		inline RenderPass& add_input_attachement(VkFormat format, VkSampleCountFlagBits samples,
			VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op,
			VkImageLayout initial_layout, VkImageLayout final_layout,
			VkImageLayout ref_layout) {
			_input_att.emplace_back(format, samples, load_op, store_op, load_op, store_op, initial_layout, final_layout, ref_layout);
			return *this;
		}

		inline RenderPass& add_color_attachement(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkImageLayout ref_layout) {
			_color_att.emplace_back(format, samples, load_op, store_op, load_op, store_op, initial_layout, final_layout, ref_layout);
			return *this;
		}

		inline RenderPass& add_resolve_attachement(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkImageLayout ref_layout) {
			_resolve_att.emplace_back(format, samples, load_op, store_op, load_op, store_op, initial_layout, final_layout, ref_layout);
			return *this;
		}

		inline RenderPass& set_depth_attachement(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkImageLayout ref_layout) {
			_depth_att = { format, samples, load_op, store_op, load_op, store_op, initial_layout, final_layout, ref_layout };
			return *this;
		}

		inline uint32_t get_all_attachment_count() const {
			return get_input_attachment_count() + get_color_attachment_count() + get_resolve_attachment_count() + get_depth_attachment_count();
		}

		inline uint32_t get_input_attachment_count() const {
			return _input_att.size_u32();
		}

		inline uint32_t get_color_attachment_count() const {
			return _color_att.size_u32();
		}

		inline uint32_t get_resolve_attachment_count() const {
			return _resolve_att.size_u32();
		}

		inline uint32_t get_depth_attachment_count() const {
			return _depth_att.has_value() ? 1 : 0;
		}

		inline void clear_attachments() {
			_input_att.clear();
			_color_att.clear();
			_resolve_att.clear();
			_depth_att.reset();
		}

		VkResult compile();

	private:
		Device* _device;
		VkRenderPass _render_pass;
		MVector<AttachmentDescription> _input_att{};
		MVector<AttachmentDescription> _color_att{};
		MVector<AttachmentDescription> _resolve_att{};
		std::optional<AttachmentDescription> _depth_att{};
	};

}