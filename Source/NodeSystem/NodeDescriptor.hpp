#pragma once

#include "Core/Inherit.hpp"
#include "NodeSystemTypes.hpp"
#include "PinDescriptor.hpp"

namespace Warp::NodeSystem {

	class PinDescriptor;
	class ContextDescriptor;

	class NodeDescriptor {
	public:
		NodeDescriptor(std::string_view name, std::string_view desc = "");

		~NodeDescriptor();

		NodeDescriptor(const NodeDescriptor&) = delete;

		NodeDescriptor(NodeDescriptor&&) = delete;

		NodeDescriptor& operator=(const NodeDescriptor&) = delete;

		NodeDescriptor& operator=(NodeDescriptor&&) = delete;

		NodeDescriptor& add_output_pin(PinType type, std::string_view name, std::string_view desc = "");

		NodeDescriptor& add_input_pin(PinType type, std::string_view name, std::string_view desc = "");

		NodeDescriptor& remove_output_pin(std::string_view name);

		NodeDescriptor& remove_input_pin(std::string_view name);

		NodeDescriptor& remove_output_pin(uint16_t idx);

		NodeDescriptor& remove_input_pin(uint16_t idx);

		NodeDescriptor& clear_output_pin();

		NodeDescriptor& clear_input_pin();

		PinDescriptor* find_output_pin(std::string_view name) const;

		PinDescriptor* find_input_pin(std::string_view name) const;

		const MVector<std::unique_ptr<PinDescriptor>>* get_output_pin() const;

		const MVector<std::unique_ptr<PinDescriptor>>* get_input_pin() const;

		void set_name(std::string_view name);

		void set_desc(std::string_view desc);

		std::string_view get_name() const;

		std::string_view get_desc() const;

		ContextDescriptor* set_context(ContextDescriptor* clone_from);

		ContextDescriptor* get_context() const;

		std::unique_ptr<NodeDescriptor> clone(std::string_view new_name = "") const;

	private:
		MVector<std::unique_ptr<PinDescriptor>> _pin_input{};
		MVector<std::unique_ptr<PinDescriptor>> _pin_output{};

		Data::DataUnion _name;
		Data::DataUnion _desc;

		std::unique_ptr<ContextDescriptor> _context;
	};

}