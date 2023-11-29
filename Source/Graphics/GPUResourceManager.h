// Purpose: GPUResourceManager header file.
// 本文件中提供了一个GPUResourceManager，用于管理GPU资源
// 以及资源查找筛选器GPUResourceSearcher

#pragma once
#include "GPUResource/GPUResourceBase.h"
#include "Core/Log.h"

#include "Core/MIStl.h"
#include "SDL3/SDL.h"

#include <tbb/spin_rw_mutex.h>

namespace Warp {
	namespace GPU {

		//Base:

		class GPUImage;
		class GPUSwapChain;
		class GPUCommandPool;
		class GPUCommandBuffer;
		class GPUFence;
		class GPUSemaphore;
		class GPURenderPass;
		class GPUGraphicsPipeline;
		class GPUShader;
		class GPUPipelineLayout;
		class GPUFrameBuffer;
		class GPUBuffer;
		class GPUDescriptorPool;
		class GPUDescriptorSetLayout;

		template<typename T> class GPUResourceManager;
		template<typename T> class GPUResourceBuilder;

		template<typename T> class GPUResourceSearchBase {
		protected:
			template<typename T> friend class GPUResourceManager;

			const MVector<std::unique_ptr<T>>& m_resources_ref{};

			GPUResourceSearchBase(const MVector<std::unique_ptr<T>>& ref) : m_resources_ref(ref) {}
		};

		template<class T> class GPUResourceSearcher : GPUResourceSearchBase<T> {
			template<typename T2> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<T>>& ref) : GPUResourceSearchBase<T>(ref) {}

			void erase_update(T* item) {}

			void add_update(T* item) {}
		};

		//Managers :

		template<>
		class GPUResourceSearcher<GPUSwapChain> : GPUResourceSearchBase<GPUSwapChain> {

			template<typename T> friend class GPUResourceManager;

			MMap<void*, uint32_t> m_window2idx{};

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUSwapChain>>& ref) : GPUResourceSearchBase(ref) {}

			void erase_update(GPUSwapChain* item);

			void add_update(GPUSwapChain* item);

		public:
			std::optional<GPUSwapChain*> find_by_window_ptr(const SDL_Window* ptr) const;
		};


		class GPUResourceManagerBase
		{
		public:
			GPUResourceManagerBase() = default;
			virtual ~GPUResourceManagerBase() = default;

			virtual void erase(void* ptr) = 0;
			virtual bool available(void* ptr) = 0;
		};

		template<typename T> class GPUResourceManager : GPUResourceManagerBase {

			MSet<T*> m_ptr_set{};

			MMap<MString, uint32_t> m_name_idx{};

			MVector<uint32_t> anonymous_idx{};

			MVector<std::unique_ptr<T>> m_resources{};

			MStack<uint32_t> m_free_space{ 64 };

			GPUResourceSearcher<T> m_searcher;

			std::atomic<uint32_t> m_resource_count{};

			const MString m_manager_name{};

		public:

			GPUResourceManager(const MString& name) : m_searcher(m_resources), m_manager_name(name) {
				GPUFactory::manager_ptr_register(this);
				anonymous_idx.reserve(64);
				m_resources.reserve(64);
				m_ptr_set.reserve(64);
				m_name_idx.reserve(64);
			}

			virtual ~GPUResourceManager() {
				clear();
				GPUFactory::manager_ptr_unregister(this);
			}

			GPUResourceBuilder<T> builder(const MString& resource_name = "") {
				return std::move(GPUResourceBuilder<T>(this, resource_name));
			}

			GPUResourceBuilder<T> builder(const MString& resource_name, uint32_t idx) {
				return std::move(GPUResourceBuilder<T>(this, to_MString(std::format("{}_{}", resource_name, idx))));
			}

			const GPUResourceSearcher<T>& searcher() const {
				return m_searcher;
			}

			constexpr const MString& get_manager_name() const {
				return m_manager_name;
			}

			void add(std::unique_ptr<T>&& item) {
				void* ptr = item.get();
				add(ptr);
				item.release();
			}

			void add(void* ptr) {

				T* item = static_cast<T*>(ptr);

				bool replaced = false;
				const MString name = item->get_name();
				if (!name.empty() && m_name_idx.contains(name)) {
					replaced = true;
					erase(name);
				}

				uint32_t emplace_idx = 0;
				if (m_free_space.empty()) {
					m_resources.emplace_back(item);
					emplace_idx = m_resources.end() - m_resources.begin() - 1;
				} else {
					emplace_idx = m_free_space.top();
					m_free_space.pop();
					m_resources[emplace_idx].reset(item);
				}

				m_resources[emplace_idx]->set_index_in_manager(emplace_idx);
				m_resources[emplace_idx]->set_manager_ptr(this);
				m_searcher.add_update(m_resources[emplace_idx].get());

				if (!m_resources[emplace_idx]->get_name().empty()) {
					m_name_idx.insert({ name, emplace_idx });
				} else {
					anonymous_idx.push_back(emplace_idx);
				}
				m_ptr_set.insert(m_resources[emplace_idx].get());
				m_resource_count += 1;

				if (replaced) {
					LOG("[Replace][Manager<{}>: \"{}\"] Add replaced name \"{}\" at {}, idx:{}.", typeid(T).name(), m_manager_name, name, (void*)(item), emplace_idx);
				} else {
					LOG("[Add][Manager<{}>: \"{}\"] Add name \"{}\" at {}, idx:{}.", typeid(T).name(), m_manager_name, name, (void*)(item), emplace_idx);
				}
			}

			void clear_anonymous_resource() {

				for (auto i : anonymous_idx) {
					m_searcher.erase_update(m_resources[i].get());
					m_resources[i].reset(nullptr);
					m_free_space.push(i);
					m_resource_count -= 1;
				}
				anonymous_idx.clear();
			}

			T* find_by_name(const MString& name) {

				if (m_name_idx.contains(name)) {
					return m_resources.at(m_name_idx.at(name)).get();
				}
				return nullptr;
			}

			MVector<T*> find_by_name_start_with(const MString& name) {

				MVector<T*> result{};
				for (auto& [key, value] : m_name_idx) {
					if (key.starts_with(name)) {
						result.push_back(m_resources.at(value).get());
					}
				}
				return result;
			}

			MVector<T*> find_by_name_end_with(const MString& name) {

				MVector<T*> result{};
				for (auto& [key, value] : m_name_idx) {
					if (key.ends_with(name)) {
						result.push_back(m_resources.at(value).get());
					}
				}
				return result;
			}

			MVector<T*> find_by_name_contain(const MString& name) {

				MVector<T*> result{};
				for (auto& [key, value] : m_name_idx) {
					if (key.contains(name)) {
						result.push_back(m_resources.at(value).get());
					}
				}
				return result;
			}

			bool available(void* ptr) override {
				return  m_ptr_set.contains(static_cast<T*>(ptr));
			}

			void erase(const MVector<T*>& ptrs) {
				for (const auto ptr : ptrs) {
					erase(ptr);
				}
			}

			void erase(const MVector<MString>& names) {
				for (const auto& ptr : names) {
					erase(ptr);
				}
			}

			void erase(void* ptr) override {
				erase(static_cast<T*>(ptr));
			}

			void erase(const T* ptr) {

				if (ptr == nullptr) {
					return;
				}

				if (!m_ptr_set.contains(ptr)) {
					LOGW("[Remove][Manager<{}>: \"{}\"] Try to delete {}, but can't find this ptr.", typeid(T).name(), m_manager_name, (void*)ptr);
					return;
				}

				uint32_t idx = ptr->get_index_in_manager();
				const MString name = ptr->get_name();

				if (!name.empty() && m_name_idx.contains(name)) {
					m_name_idx.erase(name);
				}

				m_searcher.erase_update(m_resources[idx].get());

				m_resources[idx].reset(nullptr);

				m_ptr_set.erase(ptr);
				m_free_space.push(idx);

				m_resource_count -= 1;
				LOG("[Remove][Manager<{}>: \"{}\"] Item {}, name \"{}\", idx:{} was deleted.", typeid(T).name(), m_manager_name, (void*)ptr, name, idx);

			}

			void erase(const MString& name) {
				if (m_name_idx.contains(name)) {
					const uint32_t idx = m_name_idx.at(name);
					erase(m_resources[idx].get());
				}
			}

			void clear() {
				for (const auto& ptr : m_resources) {
					erase(ptr.get());
				}
				m_resources.clear();
				m_resource_count = 0;
				m_free_space.clear();
				anonymous_idx.clear();
				if (!m_name_idx.empty() || !m_ptr_set.empty()) {
					LOGE("[Remove][Manager<{}>: \"{}\"] clear failed, unknown item info can't be clear completed.", typeid(T).name(), m_manager_name);
				}
			}

			uint32_t get_resource_count() {
				return m_resource_count;
			}

			bool is_empty() {
				return m_resource_count == 0;
			}
		};
	}
}