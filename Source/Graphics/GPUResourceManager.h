// Purpose: GPUResourceManager header file.
// 本文件中提供了一个GPUResourceManager，用于管理GPU资源
// 以及资源查找筛选器GPUResourceSearcher

#pragma once
#include "GPUResourceBase.h"
#include "Core/Log.h"

#include "Core/MIStl.h"
#include "SDL3/SDL.h"

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

		template<typename T> class GPUResourceManager;

		template<typename T> class GPUResourceBuilder;

		template<class T> class GPUResourceSearcher {};

		template<typename T> class GPUResourceTagSearcher {
		protected:
			template<typename T> friend class GPUResourceManager;

			const MVector<std::unique_ptr<T>>& m_resources_ref{};

			GPUResourceTagSearcher(const MVector<std::unique_ptr<T>>& ref) : m_resources_ref(ref) {}

			void erase_update(T* item) {

			}

			void add_update(T* item) {

			}
		};


		//Managers :


		template<> class GPUResourceSearcher<GPUBuffer> : GPUResourceTagSearcher<GPUBuffer> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUBuffer>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUBuffer* item) {
				//TODO
			}

			void add_update(GPUBuffer* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUFrameBuffer> : GPUResourceTagSearcher<GPUFrameBuffer> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUFrameBuffer>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUFrameBuffer* item) {
				//TODO
			}

			void add_update(GPUFrameBuffer* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUPipelineLayout> : GPUResourceTagSearcher<GPUPipelineLayout> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUPipelineLayout>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUPipelineLayout* item) {
				//TODO
			}

			void add_update(GPUPipelineLayout* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUShader> : GPUResourceTagSearcher<GPUShader> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUShader>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUShader* item) {
				//TODO
			}

			void add_update(GPUShader* item) {

			}

		public:

		};

		template<> class GPUResourceSearcher<GPUGraphicsPipeline> : GPUResourceTagSearcher<GPUGraphicsPipeline> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUGraphicsPipeline>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUGraphicsPipeline* item) {
				//TODO
			}

			void add_update(GPUGraphicsPipeline* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPURenderPass> : GPUResourceTagSearcher<GPURenderPass> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPURenderPass>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPURenderPass* item) {
				//TODO
			}

			void add_update(GPURenderPass* item) {

			}

		public:

		};

		template<> class GPUResourceSearcher<GPUSemaphore> : GPUResourceTagSearcher<GPUSemaphore> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUSemaphore>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUSemaphore* item) {
				//TODO
			}

			void add_update(GPUSemaphore* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUFence> : GPUResourceTagSearcher<GPUFence> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUFence>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUFence* item) {
				//TODO
			}

			void add_update(GPUFence* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUCommandBuffer> : GPUResourceTagSearcher<GPUCommandBuffer> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUCommandBuffer>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUCommandBuffer* item) {
				//TODO
			}

			void add_update(GPUCommandBuffer* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUCommandPool> : GPUResourceTagSearcher<GPUCommandPool> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUCommandPool>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUCommandPool* item) {
				//TODO
			}

			void add_update(GPUCommandPool* item) {

			}

		public:

		};


		template<> class GPUResourceSearcher<GPUImage> : GPUResourceTagSearcher<GPUImage> {
			template<typename T> friend class GPUResourceManager;

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUImage>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUImage* item) {
				//TODO

			}

			void add_update(GPUImage* item) {

			}
		public:

		};

		template<> class GPUResourceSearcher<GPUSwapChain> : GPUResourceTagSearcher<GPUSwapChain> {

			template<typename T> friend class GPUResourceManager;

			MMap<void*, uint32_t> m_window2idx{};

			GPUResourceSearcher(const MVector<std::unique_ptr<GPUSwapChain>>& ref) : GPUResourceTagSearcher(ref) {}

			void erase_update(GPUSwapChain* item);

			void add_update(GPUSwapChain* item);

		public:
			std::optional<GPUSwapChain*> find_by_window_ptr(const SDL_Window* ptr) const;
		};

		template<typename T> class GPUResourceManager {

			MSet<T*> m_ptr_set{};

			MMap<MString, uint32_t> m_name_idx{};

			MVector<uint32_t> anonymous_idx{};

			MVector<std::unique_ptr<T>> m_resources{};

			MStack<uint32_t> m_free_space{};

			GPUResourceSearcher<T> m_searcher;

			uint32_t m_resource_count{};

			const MString m_manager_name{};

		public:

			GPUResourceManager(const MString& name) : m_searcher(m_resources), m_manager_name(name) {
				GPUFactory::manager_ptr_register(this);
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
				bool replaced = false;
				const MString name = item->get_name();
				if (!name.empty() && find_by_name(name)) {
					replaced = true;
					erase(name);
				}
				if (replaced) {
					LOG("[Replace][Manager<{}>: \"{}\"] Add replaced name \"{}\" at {}.", typeid(T).name(), m_manager_name, name, (void*)(item.get()));
				} else {
					LOG("[Add][Manager<{}>: \"{}\"] Add name \"{}\" at {}.", typeid(T).name(), m_manager_name, name, (void*)(item.get()));
				}

				uint32_t emplace_idx = 0;
				if (m_free_space.empty()) {
					m_resources.push_back(std::move(item));
					emplace_idx = m_resources.end() - m_resources.begin() - 1;
				} else {
					emplace_idx = m_free_space.top();
					m_free_space.pop();
					m_resources[emplace_idx] = std::move(item);
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
				m_resource_count++;
			}

			void clear_anonymous_resource() {
				for (auto i : anonymous_idx) {
					m_searcher.erase_update(m_resources[i].get());
					m_searcher.GPUResourceTagSearcher<T>::erase_update(m_resources[i].get());
					m_resources[i].reset(nullptr);
					m_free_space.push(i);
					m_resource_count--;
				}
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

			bool available(const T* ptr) {
				return m_ptr_set.contains(ptr);
			}

			void erase(const T* ptr) {
				if (ptr == nullptr) {
					return;
				}
				if (!available(ptr)) {
					LOGW("[Remove][Manager<{}>: \"{}\"] Try to delete {}, but can't find this ptr.", typeid(T).name(), m_manager_name, (void*)ptr);
					return;
				}

				uint32_t idx = ptr->get_index_in_manager();
				const MString name = ptr->get_name();

				if (!name.empty() && m_name_idx.contains(name)) {
					m_name_idx.erase(name);
				}

				m_searcher.erase_update(m_resources[idx].get());
				m_searcher.GPUResourceTagSearcher<T>::erase_update(m_resources[idx].get());

				m_resources[idx].reset(nullptr);

				m_ptr_set.erase(ptr);
				m_free_space.push(idx);

				m_resource_count--;
				LOG("[Remove][Manager<{}>: \"{}\"] Item {}, name \"{}\" was deleted.", typeid(T).name(), m_manager_name, (void*)ptr, name);

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
				m_free_space = {};
				if (!m_name_idx.empty() || !m_ptr_set.empty()) {
					LOGE("[Remove][Manager<{}>: \"{}\"] clear failed, unknown item info can't be clear completed.", typeid(T).name(), m_manager_name);
				}

			}

			uint32_t get_resource_count() const {
				return m_resource_count;
			}

			bool is_empty() {
				return m_resource_count == 0;
			}
		};
	}
}