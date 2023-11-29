#pragma once
#include "Core/MIStl.h"

#include "Graphics/GPUResource.h"

namespace Warp::Render {

	class RenderTaskGraph;

	class TaskNodeBase {
	public:
		inline TaskNodeBase(RenderTaskGraph* render_system,
			const MVector<GPU::GPUImage*>& ref_images,
			const MVector<GPU::GPUBuffer*>& ref_buffers) :m_ref_images(ref_images), m_ref_buffers(ref_buffers), m_render_system(render_system){}
		TaskNodeBase(const TaskNodeBase&) = delete;
		TaskNodeBase(TaskNodeBase&&) = delete;
		TaskNodeBase& operator=(const TaskNodeBase&) = delete;
		TaskNodeBase& operator=(TaskNodeBase&&) = delete;
		virtual ~TaskNodeBase() = default;

		virtual bool prepare() = 0;
		virtual void generate_commands(GPU::GPUCommandBuffer* commandbuf) = 0;

		MVector<GPU::GPUImage*> m_ref_images{};
		MVector<GPU::GPUBuffer*> m_ref_buffers{};
		GPU::GPUResourceHandle<GPU::GPUFrameBuffer> m_frame_buffer{};
		GPU::GPURenderPass* m_render_pass{};
		RenderTaskGraph* m_render_system{};
	};


	class TaskNode_1 : public TaskNodeBase {
	public:

		inline TaskNode_1(RenderTaskGraph* render_system,
			const MVector<GPU::GPUImage*>& ref_images,
			const MVector<GPU::GPUBuffer*>& ref_buffers) :TaskNodeBase(render_system, ref_images, ref_buffers) {}

		TaskNode_1(const TaskNode_1&) = delete;
		TaskNode_1(TaskNode_1&&) = delete;
		TaskNode_1& operator=(const TaskNode_1&) = delete;
		TaskNode_1& operator=(TaskNode_1&&) = delete;
		~TaskNode_1() override = default;

		bool prepare() override;

		void generate_commands(GPU::GPUCommandBuffer* commandbuf) override {

		}
	};

	struct RenderTaTaskNodeNameMapper {

		static inline MMap<MString, std::function<std::unique_ptr<TaskNodeBase>(RenderTaskGraph*,
			const MVector<GPU::GPUImage*>&,
			const MVector<GPU::GPUBuffer*>&)>> name_2_task_node{
			{"TaskNode_1", [](RenderTaskGraph* render_system, const MVector<GPU::GPUImage*>& ref_images, const MVector<GPU::GPUBuffer*>&ref_buffers) {
				return std::unique_ptr<TaskNodeBase>((TaskNodeBase*)(new TaskNode_1(render_system, ref_images, ref_buffers)));
			}}
		};
	};
};
