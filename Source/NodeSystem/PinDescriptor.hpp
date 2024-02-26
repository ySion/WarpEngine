#pragma once

#include "Core/Core.hpp"
#include "NodeSystemTypes.hpp"

namespace Warp::NodeSystem {

	class NodeDescriptor;

	class PinDescriptor {
	public:

		PinDescriptor() = delete;

		~PinDescriptor();

		PinDescriptor(NodeDescriptor* parent_node, PinType pin_type, std::string_view name, std::string_view desc = "");

		PinDescriptor(const PinDescriptor&) = delete;

		PinDescriptor(PinDescriptor&&) = delete;

		PinDescriptor& operator=(const PinDescriptor&) = delete;

		PinDescriptor& operator=(PinDescriptor&&) = delete;

		bool connect(PinDescriptor* target, bool allow_replace = false);

		void disconnect_all();

		bool disconnect(uint16_t idx);

		bool disconnect(const PinDescriptor* pin);

		const MVector<PinDescriptor*>* get_connected_pins() const;

		PinDescriptor* get_connected_pin(uint16_t idx = 0) const;

		std::optional<uint16_t> get_connected_pin_idx(const PinDescriptor* target) const;

		bool is_contains_pin(const PinDescriptor* pin) const;

		void set_pin_name(std::string_view name);

		void set_pin_desc(std::string_view desc);

		std::string_view get_name() const;

		std::string_view get_desc() const;

		PinType get_pin_type() const;

		NodeDescriptor* get_parent() const;

	private:
		bool be_disconnected(const PinDescriptor* pin);

		void be_connected(PinDescriptor* pin);

		//TODO: 次序调整实现

	private:

		Data::DataUnion _name;

		Data::DataUnion _desc;

		PinType _pin_type = PinType::Single;

		NodeDescriptor* _parent_node{};

		MVector<PinDescriptor*> _connected{};
	};
}