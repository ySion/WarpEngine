#pragma once

#include "Data/Data.hpp"
#include "Managers//DensifySystemManager.hpp"
#include "Managers//DataLayoutManager.hpp"

namespace Warp::Engine {

	class EngineContext {
	public:
		EngineContext() {
			_data_layout_manager = std::make_unique<Data::DataLayoutManager>();
			_densify_system_manager = std::make_unique<DensifySystem::DensifySystemManager>();
		}

		~EngineContext() = default;

		EngineContext(const EngineContext&) = delete;
		EngineContext(EngineContext&&) = delete;

		EngineContext& operator=(const EngineContext&) = delete;
		EngineContext& operator=(EngineContext&&) = delete;

		inline Data::DataLayoutManager* get_data_layout_manager() const {
			return _data_layout_manager.get();
		}

		inline DensifySystem::DensifySystemManager* get_densify_system_manager() const {
			return _densify_system_manager.get();
		}

	private:
		std::unique_ptr<Data::DataLayoutManager> _data_layout_manager{};
		std::unique_ptr<DensifySystem::DensifySystemManager> _densify_system_manager{};
	};
}
