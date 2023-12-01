#include "GPUResourceManager.h"
#include "GPUResource.h"

void Warp::GPU::GPUResourceSearcher<Warp::GPU::GPUSwapChain>::erase_update(GPUSwapChain* item)
{
	if (m_window2idx.contains(item->m_window)) {
		m_window2idx.erase(item->m_window);
	}
}

void Warp::GPU::GPUResourceSearcher<Warp::GPU::GPUSwapChain>::add_update(GPUSwapChain* item)
{
	m_window2idx.insert({ item->m_window , item->get_index_in_manager() });
}

std::optional<Warp::GPU::GPUSwapChain*> Warp::GPU::GPUResourceSearcher<Warp::GPU::GPUSwapChain>::find_by_window_ptr(
	const SDL_Window* ptr) const
{
	tbb::spin_rw_mutex::scoped_lock lock(m_mutex, false);
	if (m_window2idx.contains(ptr)) {
		uint32_t idx = m_window2idx.at(ptr);
		return { m_resources_ref[idx].get() };
	}
	return std::nullopt;
}
