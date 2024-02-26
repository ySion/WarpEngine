#include "NodeDescriptor.hpp"

#include "PinDescriptor.hpp"
#include "ContextDescriptor.hpp"

using namespace Warp::NodeSystem;

NodeDescriptor::NodeDescriptor(std::string_view name, std::string_view desc) {
	_name = name;
	_desc = desc;
	_context = std::make_unique<ContextDescriptor>(); // defualt context
}

NodeDescriptor::~NodeDescriptor() {
	_pin_input.clear();
	_pin_output.clear();
}

NodeDescriptor& NodeDescriptor::add_output_pin(PinType type, std::string_view name, std::string_view desc) {
	for (const auto& pin : _pin_output) {
		if (pin->get_name() == name) {
			MString msg = MString::format("Warp::NodeSystem::NodeDescriptor::add_output_pin failed: {}, pin is already exists.", name);
			error(msg);
			return *this;
		}
	}
	_pin_output.push_back(std::make_unique<PinDescriptor>(this, type, name, desc));
	return *this;
}

NodeDescriptor& NodeDescriptor::add_input_pin(PinType type, std::string_view name, std::string_view desc) {
	for (const auto& pin : _pin_input) {
		if (pin->get_name() == name) {
			MString msg = MString::format("Warp::NodeSystem::NodeDescriptor::add_input_pin failed: {}, pin is already exists.", name);
			error(msg);
			return *this;
		}
	}
	_pin_input.push_back(std::make_unique<PinDescriptor>(this, type, name, desc));
	return *this;
}

NodeDescriptor& NodeDescriptor::remove_output_pin(std::string_view name) {
	for (uint16_t i = 0; i < _pin_output.size_u16(); ++i) {
		if (_pin_output[i]->get_name() == name) {
			_pin_output.erase(_pin_output.begin() + i);
			return *this;
		}
	}
	return *this;
}

NodeDescriptor& NodeDescriptor::remove_input_pin(std::string_view name) {
	for (uint16_t i = 0; i < _pin_input.size_u16(); ++i) {
		if (_pin_input[i]->get_name() == name) {
			_pin_input.erase(_pin_input.begin() + i);
			return *this;
		}
	}
	return *this;
}

NodeDescriptor& NodeDescriptor::remove_output_pin(uint16_t idx) {
	if (idx < _pin_output.size_u16()) {
		_pin_output.erase(_pin_output.begin() + idx);
	}
	return *this;
}

NodeDescriptor& NodeDescriptor::remove_input_pin(uint16_t idx) {
	if (idx < _pin_input.size_u16()) {
		_pin_input.erase(_pin_input.begin() + idx);
	}
	return *this;
}

NodeDescriptor& NodeDescriptor::clear_output_pin() {
	_pin_output.clear();
	return *this;
}

NodeDescriptor& NodeDescriptor::clear_input_pin() {
	_pin_input.clear();
	return *this;
}

PinDescriptor* NodeDescriptor::find_output_pin(std::string_view name) const {
	for (const auto& pin : _pin_output) {
		if (pin->get_name() == name) {
			return pin.get();
		}
	}
	return nullptr;
}

PinDescriptor* NodeDescriptor::find_input_pin(std::string_view name) const {
	for (const auto& pin : _pin_input) {
		if (pin->get_name() == name) {
			return pin.get();
		}
	}
	return nullptr;
}

const MVector<std::unique_ptr<PinDescriptor>>* NodeDescriptor::get_output_pin() const {
	return &_pin_output;
}

const MVector<std::unique_ptr<PinDescriptor>>* NodeDescriptor::get_input_pin() const {
	return &_pin_input;
}

void NodeDescriptor::set_name(std::string_view name) {
	if (_name.get_text() != name) _name.set_text(name);
}

void NodeDescriptor::set_desc(std::string_view desc) {
	_desc.set_text(desc);
}

std::string_view NodeDescriptor::get_name() const {
	return _name.get_text();
}

std::string_view NodeDescriptor::get_desc() const {
	return _desc.get_text();
}

std::unique_ptr<NodeDescriptor> NodeDescriptor::clone(std::string_view new_name) const {
	std::string_view new_name_final = new_name;
	if (new_name.empty()) {
		new_name_final = _name.get_text();
	}

	auto node = std::make_unique<NodeDescriptor>(new_name_final, _desc.get_text());
	for (const auto& pin : _pin_input) {
		node->add_input_pin(pin->get_pin_type(), pin->get_name(), pin->get_desc());
	}

	for (const auto& pin : _pin_output) {
		node->add_output_pin(pin->get_pin_type(), pin->get_name(), pin->get_desc());
	}

	return node;
}

ContextDescriptor* NodeDescriptor::set_context(ContextDescriptor* clone_from) {
	if(clone_from) {
		_context = clone_from->clone();
		return _context.get();
	}

	MString msg = MString::format("Warp::NodeSystem::NodeDescriptor::set_context failed: clone_from is nullptr.");
	error(msg);
	return nullptr;
}

ContextDescriptor* NodeDescriptor::get_context() const {
	return _context.get();
}
