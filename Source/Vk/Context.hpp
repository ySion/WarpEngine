#pragma once

#include "Core/Object.hpp"
#include "Core/Inherit.hpp"


#include "Instance.hpp"
#include "Device.hpp"
#include "Window.hpp"
#include "CommandSubmitQueue.hpp"


WARP_TYPE_NAME_2(Gpu, Context)

namespace Warp::Gpu {

	using Names = std::vector<const char*>;

	class Context : public Inherit<Context, Object> {
	public:
		
		Context();

		~Context() override = default;

		inline Instance* get_instance() const { return _instance.get(); }

		inline Device* get_device() const { return _device.get(); }

		inline CommandSubmitQueue* get_command_submit_queue() const { return _command_submit_queue.get(); }

		std::unique_ptr<Window> create_window(const MString& title, int w, int h, uint32_t flag);
	private:
		std::unique_ptr<Instance> _instance{};

		std::unique_ptr<Device> _device{};

		std::unique_ptr<CommandSubmitQueue> _command_submit_queue{};
	};
}
