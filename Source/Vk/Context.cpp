#include "Context.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"
#include "Core/observer_ptr.hpp"

using namespace Warp::Gpu;

Context::Context()
{
	std::vector<const char*> instance_extensions {
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	std::vector<const char*> instance_layers {
		"VK_LAYER_KHRONOS_validation"
	};

	_instance = std::make_unique<Instance>(instance_extensions, instance_layers);

	const auto& physical_device_ref = _instance->get_physical_devices();

	observer_ptr<PhysicalDevice> physical_device_ptr = nullptr;
	for (auto& physical_device : physical_device_ref) {
		if (physical_device->is_queue_family0_support_all_queue()) {
			physical_device_ptr = physical_device.get();
			break;
		}
	}

	if (!physical_device_ptr.is_object_vaild()) {
		MString error_msg = "No physical device support all queue family";
		error(error_msg);
		throw Exception{ error_msg, 0};
	}

	std::vector device_extension_names = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	_device = std::make_unique<Device>(physical_device_ptr.get(), device_extension_names);

	//3重缓冲
	_command_submit_queue = std::make_unique<CommandSubmitQueue>(_device.get(), 3);
}

std::unique_ptr<Window> Context::create_window(const MString& title, int w, int h, uint32_t flag) {
	return std::make_unique<Window>(this, title, w, h, flag);
}