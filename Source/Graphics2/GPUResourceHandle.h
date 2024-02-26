#pragma once
#include "GPUResourceManager.h"

namespace Warp {
	namespace GPU {
		class GPUResource;

		template<class T>
			requires std::is_base_of_v<GPUResource, T>
		class GPUResourceHandle {
		public:

			GPUResourceHandle(T* resource = nullptr) : handle(resource) {}
			
			~GPUResourceHandle() {
				if (available()) {
					static_cast<GPUResourceManagerBase*>(handle->m_manager_ptr)->erase(handle);
				}
			}

			GPUResourceHandle(const GPUResourceHandle&) = delete;

			GPUResourceHandle& operator=(const GPUResourceHandle&) = delete;

			GPUResourceHandle(GPUResourceHandle&& other) noexcept
				: handle(other.handle)
			{
				other.handle = nullptr;
			}

			GPUResourceHandle& operator=(GPUResourceHandle&& other) noexcept {
				handle = other.handle;
				other.handle = nullptr;
				return *this;
			}

			template<class T2> T2* cast_to() {
				return static_cast<T2*>(handle);
			}

			bool available() {
				if(is_manager_available()){
					return static_cast<GPUResourceManagerBase*>(handle->m_manager_ptr)->available(handle);
				}
				return false;
			}

			bool is_manager_available(){
				if (handle) {
					return GPUFactory::manager_ptr_is_register_available(handle->m_manager_ptr);
				}
				return false;
			}

			constexpr T* get() const { return handle; }

			constexpr void reset(T* ptr = nullptr, bool kill_origin = true) {
				if (kill_origin) {
					if(available()){
						static_cast<GPUResourceManagerBase*>(handle->m_manager_ptr)->erase(handle);
					}
					handle = ptr;
				}
				else {
					handle = ptr;
				}
			}

			constexpr void release(T* ptr = nullptr) {
				handle = nullptr;
			}

			constexpr T* operator->() const { return get(); }

			constexpr T& operator*() const { return *handle; }

			constexpr operator bool() const { return handle != nullptr; }

			constexpr bool operator!() const { return handle == nullptr; }
		private:
			T* handle;
		};
	}
}