#pragma once
#include <string>

#include "Graphics/GPUFactory.h"
#include "Core/MIStl.h"
#include "Core/WObject.h"

#include "Graphics/VulkanFiles.h"

namespace Warp
{
	namespace GPU
	{

		class GPUResource : public WObject {
		public:

			template<class T>
				requires std::is_base_of_v<GPUResource, T>
			friend class GPUResourceHandle;

			template<typename T> friend class GPUResourceManager;

			inline GPUResource(const MString& type, const MString& name)
				: m_type(type), m_name(name) {}

			virtual ~GPUResource() = default;

			GPUResource(const GPUResource&) = delete;

			GPUResource& operator=(const GPUResource&) = delete;

			GPUResource(GPUResource&&) = delete;

			GPUResource& operator=(GPUResource&&) = delete;

			constexpr const MString& get_type() const { return m_type; }

			constexpr const MString& get_name() const { return m_name; }

			constexpr bool is_anonymous() const { return m_name.empty(); }

			constexpr uint32_t get_index_in_manager() const { return m_index; }

			constexpr void* get_manager_ptr() const { return m_manager_ptr; }

		private:

			constexpr void set_type(const MString& name) { m_type = name; }

			constexpr void set_name(const MString& name) { m_name = name; }

			constexpr void set_index_in_manager(uint32_t idx) { m_index = idx; }

			constexpr void set_manager_ptr(void* ptr) { m_manager_ptr = ptr; }

		private:
			MString m_type; // if m_type is "", so this is invalid resource.
			MString m_name; // if m_name is "", so this is a anonymous resource.
			uint32_t m_index;
			void* m_manager_ptr{};
		};

		inline void wait_device_idle() {
			vkDeviceWaitIdle(GPUFactory::get_device());
		}
	}
}
