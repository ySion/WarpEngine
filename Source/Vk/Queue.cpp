#include "Queue.hpp"

#include "Core/Logger.hpp"

using namespace Warp;

Gpu::Queue::Queue(VkQueue queue): _queue(queue)
{
	info("Queue created");
}
