#pragma once

#include "Core/Core.hpp"

#include "NodeSystemTypes.hpp"

namespace Warp::NodeSystem {

	class NodeDescriptor;

	class ContextDescriptor {
	public:
		enum class Sequence {
			Program, // 向下递推 程序
			Value,   // 向上递推 求值
		};

		ContextDescriptor() = default;

		virtual ~ContextDescriptor() = default;

		ContextDescriptor(const ContextDescriptor&) = delete;

		ContextDescriptor(ContextDescriptor&&) = delete;

		ContextDescriptor& operator=(const ContextDescriptor&) = delete;

		ContextDescriptor& operator=(ContextDescriptor&&) = delete;

		std::unique_ptr<ContextDescriptor> clone();

		//复制某个节点到当前图中, 而不是移动(剪切), 这是一个很常用的方法, 比如从Context模板中拷贝一个样本过来, 它会在实例化中大量使用
		std::optional<NodeDescriptor*> add_node_with_clone(const NodeDescriptor* node, std::string_view new_name = "");

		//移动(剪切)某个节点到当前图中, 而不是拷贝
		std::optional<NodeDescriptor*> add_node_with_move(std::unique_ptr<NodeDescriptor> node, std::string_view new_name = "");

		std::optional<NodeDescriptor*> add_node(std::string_view name, std::string_view desc = "");

		bool remove_node(std::string_view name);

		bool clear_node_output_pin(std::string_view target, std::string_view output_pin);

		bool clear_node_input_pin(std::string_view target, std::string_view input_pin);

		bool add_edge(std::string_view from, std::string_view output_pin_from, std::string_view to, std::string_view input_pin_to);

		bool remove_edge(std::string_view from, std::string_view output_pin_from, std::string_view to, std::string_view input_pin_to);

		//判断某个节点是否存在环
		bool is_graph_node_cycling(std::string node_name, Sequence sequence);

		//判断图内是否存在环 (检查所有节点是否有环)
		bool is_graph_has_cycling(Sequence sequence);

		bool is_graph_has_depth_cycling();

		void visit(const MString& name, Sequence sequence);

	private:
		bool check_has_cycling_impl(Sequence sequence, MMap<MString, uint32_t>& depth_color, std::string_view node_name, uint32_t depth);

		bool is_graph_has_depth_cycling_impl(MSet<ContextDescriptor*>& set_list);

	private:
		ContextType _ctx_type;

		MMap<MString, std::unique_ptr<NodeDescriptor>> _node_map;
	};

}