#pragma once

#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, ImageView);

namespace Warp::Gpu {

	class Image;
	class Device;

	class ImageView : public Inherit<ImageView, Object> {
	public:

		~ImageView() override;

		inline VkImageView vk() const { return _view; }

		operator VkImageView() const { return _view; }

	private:
		ImageView(Image* image, VkImageView image_view);

		VkImageView _view;

		Image* _image;

		friend class Image;
	};
}
