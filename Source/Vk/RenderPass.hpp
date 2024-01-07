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
		MVector<AttachmentDescription> _input_att {};
		MVector<AttachmentDescription> _color_att {};
		MVector<AttachmentDescription> _resolve_att {};
		std::optional<AttachmentDescription> _depth_att {};
	};

}