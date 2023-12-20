#pragma once
#include "VkBase.hpp"

#include "Core/TypeName.hpp"
#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, Queue)

namespace Warp::Gpu {

	class Device;

	class Queue : public Inherit<Queue, Object> {
	public:
		~Queue() override = default;

		operator VkQueue() const { return _queue; }

		inline VkQueue vk() const { return  _queue; }

	protected:
		Queue(VkQueue queue);

		VkQueue _queue{};

		friend class Device;
	};
}