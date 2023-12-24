#include "ImageView.hpp"

#include "Device.hpp"
#include "Image.hpp"


using namespace Warp::Gpu;

ImageView::ImageView(Image* image, VkImageView image_view) :  _view(image_view), _image(image) {
		
}

ImageView::~ImageView()
{
	if (_view) {
		vkDestroyImageView(_image->get_device()->vk(), _view, nullptr);
	}
}
