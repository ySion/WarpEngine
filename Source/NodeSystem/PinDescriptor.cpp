#include "PinDescriptor.hpp"

#include "NodeDescriptor.hpp"

using namespace Warp::NodeSystem;

PinDescriptor::~PinDescriptor() {
	disconnect_all();
}

PinDescriptor::PinDescriptor(NodeDescriptor* parent_node, NodeSystem::PinType pin_type, std::string_view name, std::string_view desc) : _name(name), _desc(desc), _pin_type(pin_type), _parent_node(parent_node) {}

bool PinDescriptor::connect(PinDescriptor* target, bool allow_replace) {
	if (target == nullptr) {
		MString msg = MString::format("Warp::NodeSystem::Pin::connect failed: node {}'s pin {} try to link to nullptr.", get_parent()->get_name(), get_name());
		error(msg);
		return false;
	}
	if (target == this) {
		MString msg = MString::format("Warp::NodeSystem::Pin::connect failed: node {}'s pin {} try to link to self.", get_parent()->get_name(), get_name());
		error(msg);
		return false;
	}
	if (is_contains_pin(target)) {
		MString msg = MString::format("Warp::NodeSystem::Pin::connect failed: node {}'s pin {} has linked {}. double link is not allowed.", get_parent()->get_name(), get_name(), target->get_name());
		error(msg);
		return false;
	}

	if (_pin_type == PinType::Single && !_connected.empty()) {
		if (!allow_replace) {
			MString msg = MString::format("Warp::NodeSystem::Pin::connect failed: node {}'s pin {} has linked to node {}'s {}. but try to redirect to {}. please enable 'allow_replace' to redirect.", get_parent()->get_name(), get_name(), _connected[0]->get_parent()->get_name(), _connected[0]->get_name(), target->get_name());
			warn(msg);
			return false;
		}
		disconnect(_connected[0]);
	}

	_connected.push_back(target);
	target->be_connected(this);
	return true;
}

void PinDescriptor::disconnect_all() {
	for (const auto& pin : _connected) {
		pin->be_disconnected(this);
	}
	_connected.clear();
}

bool PinDescriptor::disconnect(uint16_t idx) {
	if (idx >= _connected.size_u16()) {
		return false;
	}
	_connected[idx]->be_disconnected(this);
	_connected.erase(_connected.begin() + idx);
	return true;
}

bool PinDescriptor::disconnect(const PinDescriptor* pin) {
	for (uint16_t i = 0; i < _connected.size_u16(); ++i) {
		if (_connected[i] == pin) {
			_connected[i]->be_disconnected(this);
			_connected.erase(_connected.begin() + i);
			return true;
		}
	}
	return false;
}

const MVector<PinDescriptor*>* PinDescriptor::get_connected_pins() const {
	return &_connected;
}

PinDescriptor* PinDescriptor::get_connected_pin(uint16_t idx) const {
	if (idx >= _connected.size_u16()) {
		return nullptr;
	}
	return _connected[0];
}

std::optional<uint16_t> PinDescriptor::get_connected_pin_idx(const PinDescriptor* target) const {
	for (uint16_t i = 0; i < _connected.size_u16(); ++i) {
		if (_connected[i] == target) {
			return i;
		}
	}
	return std::nullopt;
}

bool PinDescriptor::is_contains_pin(const PinDescriptor* pin) const {
	for (const auto& p : _connected) {
		if (p == pin) {
			return true;
		}
	}
	return false;
}

NodeDescriptor* PinDescriptor::get_parent() const {
	return _parent_node;
}

std::string_view PinDescriptor::get_name() const {
	return _name.get_text();
}

std::string_view PinDescriptor::get_desc() const {
	return _desc.get_text();
}

void PinDescriptor::set_pin_name(std::string_view name) {
	_name.set_text(name);
}

void PinDescriptor::set_pin_desc(std::string_view desc) {
	_desc.set_text(desc);
}

PinType PinDescriptor::get_pin_type() const {
	return _pin_type;
}

bool PinDescriptor::be_disconnected(const PinDescriptor* pin) {
	for (uint16_t i = 0; i < _connected.size_u16(); ++i) {
		if (_connected[i] == pin) {
			_connected.erase(_connected.begin() + i);
			return true;
		}
	}
	return false;
}

void PinDescriptor::be_connected(PinDescriptor* pin) {
	if (_pin_type == PinType::Single && !_connected.empty()) {
		disconnect(_connected[0]);
	}
	_connected.push_back(pin);
}
