#include "ContextDescriptor.hpp"

#include "PinDescriptor.hpp"
#include "NodeDescriptor.hpp"

using namespace Warp::NodeSystem;

std::unique_ptr<ContextDescriptor> ContextDescriptor::clone() {
	auto clone_context = std::make_unique<ContextDescriptor>();
	for (const auto& [name, node] : _node_map) {
		clone_context->add_node_with_clone(node.get());
	}

	for (const auto& [name, node] : _node_map) {
		////connect output 不使用ouput, 因为它和主动创建过程不一致, 主动创建过程是output -> input, 这里是input -> output
		//for (const auto& pin : *node->get_output_pin()) {
		//	for (const auto& target_pin : *pin->get_connected_pins()) {
		//		clone_context->add_edge(node->get_name(), pin->get_name(), target_pin->get_parent()->get_name(), target_pin->get_name());
		//	}
		//}

		//connect input 这里使用input连接, 保证Width和Thin的Pin内的线连接顺序一致 (和主动创建的过程是一致的, 都是output -> input)
		for (const auto& pin : *node->get_input_pin()) {
			for (const auto& target_pin : *pin->get_connected_pins()) {
				clone_context->add_edge(target_pin->get_parent()->get_name(), target_pin->get_name(), node->get_name(), pin->get_name());
			}
		}
	}
	return clone_context;
}

std::optional<NodeDescriptor*> ContextDescriptor::add_node_with_clone(const NodeDescriptor* node,
	std::string_view new_name) {
	if (!node) return std::nullopt;
	std::string_view new_name_final = new_name.empty() ? node->get_name() : new_name;

	if (_node_map.contains(new_name_final)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::find_node failed: {}, node is already exists.", new_name_final);
		error(msg);
		return std::nullopt;
	}

	auto ptr = node->clone(new_name_final);
	auto ptr_weak = ptr.get();
	_node_map[new_name_final] = std::move(ptr);
	return ptr_weak;
}

std::optional<NodeDescriptor*> ContextDescriptor::add_node_with_move(std::unique_ptr<NodeDescriptor> node,
	std::string_view new_name) {
	if (!node) return std::nullopt;
	std::string_view new_name_final = new_name.empty() ? node->get_name() : new_name;
	if (_node_map.contains(new_name_final)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_node failed: {}, node is already exists.", new_name_final);
		error(msg);
		return std::nullopt;
	}
	node->set_name(new_name_final);
	auto node_ptr = node.get();
	_node_map[new_name_final] = std::move(node);
	return node_ptr;
}

std::optional<NodeDescriptor*> ContextDescriptor::add_node(std::string_view name, std::string_view desc) {
	if (_node_map.contains(name)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_node failed: {}, node is already exists.", name);
		error(msg);
		return std::nullopt;
	}
	auto node = std::make_unique<NodeDescriptor>(name, desc);
	auto node_ptr = node.get();
	_node_map[name] = std::move(node);
	return node_ptr;
}

bool ContextDescriptor::remove_node(std::string_view name) {
	if (!_node_map.contains(name)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::remove_node failed: can't find node {}.", name);
		error(msg);
		return false;
	}
	_node_map.erase(name);
	return true;
}

bool ContextDescriptor::clear_node_output_pin(std::string_view target, std::string_view output_pin) {
	if (!_node_map.contains(target)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::clear_node_output_pin failed: can't find node {}.", target);
		error(msg);
		return false;
	}
	_node_map[target]->clear_output_pin();
	return true;
}

bool ContextDescriptor::clear_node_input_pin(std::string_view target, std::string_view input_pin) {
	if (!_node_map.contains(target)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::clear_node_input_pin failed: can't find node {}.", target);
		error(msg);
		return false;
	}
	_node_map[target]->clear_input_pin();
	return true;
}

bool ContextDescriptor::add_edge(std::string_view from, std::string_view output_pin_from, std::string_view to,
	std::string_view input_pin_to) {
	if (!_node_map.contains(from)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_edge failed: can't find node {}.", from);
		error(msg);
		return false;
	}
	if (!_node_map.contains(to)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_edge failed: can't find node {}.", to);
		error(msg);
		return false;
	}

	const auto pfrom = _node_map[from].get();
	const auto from_pin = pfrom->find_output_pin(output_pin_from);

	const auto pto = _node_map[to].get();
	const auto to_pin = pto->find_input_pin(input_pin_to);

	if (from_pin == nullptr) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_edge failed: at node {}, can't find from_pin {}.", from, output_pin_from);
		error(msg);
		return false;
	}

	if (to_pin == nullptr) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::add_edge failed: at node {}, can't find pin {}.", to, input_pin_to);
		error(msg);
		return false;
	}

	return from_pin->connect(to_pin);
}

bool ContextDescriptor::remove_edge(std::string_view from, std::string_view output_pin_from, std::string_view to,
	std::string_view input_pin_to) {
	if (!_node_map.contains(from)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::remove_edge failed: can't find node {}.", from);
		error(msg);
		return false;
	}
	if (!_node_map.contains(to)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::remove_edge failed: can't find node {}.", to);
		error(msg);
		return false;
	}

	const auto pfrom = _node_map[from].get();
	const auto from_pin = pfrom->find_output_pin(output_pin_from);


	const auto pto = _node_map[to].get();
	const auto to_pin = pto->find_input_pin(input_pin_to);

	if (from_pin == nullptr) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::remove_edge failed: at node {}, can't find from_pin {}.", from, output_pin_from);
		error(msg);
		return false;
	}

	if (to_pin == nullptr) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::remove_edge failed: at node {}, can't find pin {}.", to, input_pin_to);
		error(msg);
		return false;
	}

	return from_pin->disconnect(to_pin);
}

bool ContextDescriptor::is_graph_node_cycling(std::string node_name, Sequence sequence) {
	if (!_node_map.contains(node_name)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::check_node_has_cycling failed: can't find node {}.", node_name);
		error(msg);
		return false;
	}
	MMap<MString, uint32_t> depth_color{};
	depth_color[node_name] = 0;
	return check_has_cycling_impl(sequence, depth_color, node_name, 1);
}

bool ContextDescriptor::is_graph_has_cycling(Sequence sequence) {
	for (const auto& [name, node] : _node_map) {
		if (is_graph_node_cycling(name, sequence)) {
			return true;
		}
	}
	return false;
}

void ContextDescriptor::visit(const MString& name, Sequence sequence) {
	if (!_node_map.contains(name)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::debug_calc_output_procedural failed: can't find node {}.", name);
		error(msg);
		return;
	}
	auto node = _node_map[name].get();
	const auto pin_list = sequence == Sequence::Program ? node->get_output_pin() : node->get_input_pin();
	info("{}.", node->get_name());
	for (const auto& pin : *pin_list) {
		for (const auto& target_pin : *pin->get_connected_pins()) {
			visit(target_pin->get_parent()->get_name(), sequence);
		}
	}
}

bool ContextDescriptor::check_has_cycling_impl(Sequence sequence, MMap<MString, uint32_t>& depth_color,
	std::string_view node_name, uint32_t depth) {
	if (!_node_map.contains(node_name)) {
		MString msg = MString::format("Warp::NodeSystem::ContextDescriptor::check_has_cycling_impl failed: can't find node {}.", node_name);
		error(msg);
		return false;
	}
	auto node = _node_map[node_name].get();
	depth_color[node_name] = depth;

	const auto pin_list = sequence == Sequence::Program ? node->get_output_pin() : node->get_input_pin();
	for (const auto& pin : *pin_list) {
		for (const auto& target_pin : *pin->get_connected_pins()) {
			MString current_node = target_pin->get_parent()->get_name();
			if (depth_color.contains(current_node)) {
				if (depth_color[current_node] < depth) {
					return true;
				}
			}

			if (check_has_cycling_impl(sequence, depth_color, current_node, depth + 1)) {
				return true;
			}
		}
	}
	return false;
}

bool ContextDescriptor::is_graph_has_depth_cycling() {
	MSet<ContextDescriptor*> set_list{};
	set_list.insert(this);
	for (const auto& [name, node] : _node_map) {
		if (node->get_context()->is_graph_has_depth_cycling_impl(set_list)) {
			return true;
		}
	}
	return false;
}

bool ContextDescriptor::is_graph_has_depth_cycling_impl(MSet<ContextDescriptor*>& set_list) {
	if (set_list.contains(this)) { return true; }
	set_list.insert(this);
	for (auto& [name, node] : _node_map) {
		if (node->get_context()->is_graph_has_depth_cycling_impl(set_list)) {
			return true;
		}
	}

	return false;
}
