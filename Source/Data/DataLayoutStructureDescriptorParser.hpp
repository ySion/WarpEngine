#pragma once

#include "Core/Core.hpp"
#include "DataTypes.hpp"

WARP_TYPE_NAME_2(Data, DataLayoutStructureDescriptorParser);

namespace Warp::Data {

	class DataLayoutStructureDescriptor;

	struct DataLayoutStructureDescriptorGroup {
		std::unique_ptr<MVector<std::unique_ptr<DataLayoutStructureDescriptor>>> _structs{};
		std::unique_ptr<MMap<MString, uint32_t>> _structs_name_to_idx{};
	};

	class DataLayoutStructureDescriptorParser : public Inherit<DataLayoutStructureDescriptorParser, Object> {

		enum class DescTextTokenType {
			Identifer,
			String,
			Int,
			Other,
			EndOfCode
		};

		enum class LineEndType {
			Normal,
			StructureEnd,
			StructureStart
		};

	public:
		inline DataLayoutStructureDescriptorParser() { _current_token.reserve(128); }

		inline void set_desc_text(std::string_view desc_text) { _desc_code = desc_text; }

		std::unique_ptr<DataLayoutStructureDescriptorGroup> parse();

	private:
		static inline bool is_space_character(char c) { return c == '\v' || c == '\f' || c == '\r' || c == '\n' || c == ' ' || c == '\t' || c == '\b'; }

		static inline bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

		static inline bool is_number(char c) { return c >= '0' && c <= '9'; }

		static inline bool is_allowed_identifer_character(char c) { return is_alpha(c) || is_number(c) || c == '_'; }

		void eat_space();

		void get_next_token();

		inline void put_token_back() { _token_position -= _current_token.length() + (_current_token_type == DescTextTokenType::String ? 2 : 0); }

		bool start_parse_struct();

		void start_parse_single(DataLayoutStructureDescriptor* struct_ptr);

		std::pair<DataPacketMemberType, MString> parse_typename();

		MVector<uint32_t> parse_array_info();

		MString parse_element_name();

		MString parse_desc_text();

		uint16_t parse_alignment();

	private:
		MString _desc_code{};
		uint32_t _token_position{};
		uint32_t _token_line{};
		MString _current_token{};
		DescTextTokenType _current_token_type{};
		DataLayoutStructureDescriptorGroup* _result{};
	};
};