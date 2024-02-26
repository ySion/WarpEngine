#include "DataLayoutStructureDescriptor.hpp"
#include "DataLayoutStructureDescriptorParser.hpp"

using namespace Warp::Data;

std::unique_ptr<DataLayoutStructureDescriptorGroup> DataLayoutStructureDescriptorParser::parse()
{
	if (_desc_code.empty()) { return nullptr; }

	_token_position = 0;
	_token_line = 1;

	auto parser_result = std::make_unique<DataLayoutStructureDescriptorGroup>();
	parser_result->_structs.reset(new MVector<std::unique_ptr<DataLayoutStructureDescriptor>>());
	parser_result->_structs_name_to_idx.reset(new MMap<MString, uint32_t>());

	_result = parser_result.get();

	try {
		while (start_parse_struct()) {}
	} catch (Exception) {
		return nullptr;
	}
	return parser_result;
}

void DataLayoutStructureDescriptorParser::eat_space()
{
	while (_token_position < _desc_code.length() && is_space_character(_desc_code[_token_position])) {
		_token_position++;
		if (_desc_code[_token_position] == '\n') _token_line++;
	}
}

void DataLayoutStructureDescriptorParser::get_next_token()
{
	_current_token = "";

	eat_space();

	if (_token_position >= _desc_code.length()) {
		_current_token_type = DescTextTokenType::EndOfCode;
		return;
	}

	if (is_number(_desc_code[_token_position])) {
		while (_token_position < _desc_code.length() && is_number(_desc_code[_token_position])) {
			_current_token += _desc_code[_token_position];
			_token_position++;
		}
		_current_token_type = DescTextTokenType::Int;
	} else if (is_allowed_identifer_character(_desc_code[_token_position])) {
		while (_token_position < _desc_code.length() && is_allowed_identifer_character(_desc_code[_token_position])) {
			_current_token += _desc_code[_token_position];
			_token_position++;
		}
		_current_token_type = DescTextTokenType::Identifer;
	} else if (_desc_code[_token_position] == '\'') {
		uint32_t mark_line = _token_line;
		_token_position++;
		while (_token_position < _desc_code.length() && _desc_code[_token_position] != '\'') {
			if (_desc_code[_token_position] == '\\') {
				_token_position++;

				if (_token_position >= _desc_code.length()) {
					MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: string not closed, string from line {}.", mark_line);
					error(msg);
					throw Exception{ msg, 0 };
				}

				switch (_desc_code[_token_position]) {
				case '\\': _current_token += '\\'; break;
				case '\'': _current_token += '\''; break;
				case 'n': _current_token += '\n'; break;
				case 't': _current_token += '\t'; break;
				default:
					MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: invaild escape character {} ,at line {}.", _desc_code[_token_position], mark_line);
					error(msg);
					throw Exception{ msg, 0 };
				}
			} else {
				_current_token += _desc_code[_token_position];
			}

			_token_position++;
		}
		if (_desc_code[_token_position] == '\'') {
			_token_position++;
		} else {
			MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: string not closed, string from line {}.", mark_line);
			error(msg);
			throw Exception{ msg, 0 };
		}
		_current_token_type = DescTextTokenType::String;

	} else {
		_current_token += _desc_code[_token_position];
		_current_token_type = DescTextTokenType::Other;
		_token_position++;
	}
}

bool DataLayoutStructureDescriptorParser::start_parse_struct()
{

	get_next_token();
	if (_current_token_type == DescTextTokenType::EndOfCode) {
		return false;
	}

	if (_current_token != "struct") {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: \"struct\" keyword to start a structure define. at line {}.", _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}

	get_next_token();
	if (_current_token_type != DescTextTokenType::Identifer) {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: expected a struct name. at line {}.", _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}

	MString struct_name = _current_token;

	if (_result->_structs_name_to_idx->contains(struct_name)) {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: struct name {} is already defined. at line {}.", _current_token.c_str(), _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}

	get_next_token();

	MString struct_desc = "";
	if (_current_token_type == DescTextTokenType::String) {
		struct_desc = _current_token;
	} else {
		put_token_back();
	}

	get_next_token();
	uint16_t struct_alignment = 1;
	if (_current_token_type == DescTextTokenType::Int) {
		struct_alignment = std::atoi(_current_token.c_str());
	} else {
		put_token_back();
	}

	get_next_token();
	if (_current_token != "{") {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: need a '{' to start a structure define. at line {}.", _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}

	get_next_token();
	if (_current_token == "}") {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: empty structure is not allowed. at line {}.", _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}
	put_token_back();

	auto desc = std::make_unique<DataLayoutStructureDescriptor>(struct_name, struct_desc, struct_alignment);

	while (_current_token != "}") {
		put_token_back();
		start_parse_single(desc.get());

		get_next_token();
		if (_current_token_type == DescTextTokenType::EndOfCode) {
			MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: need a '}' to end a structure define. at line {}.", _token_line);
			error(msg);
			throw Exception{ msg, 0 };
		}
	}

	desc->compile();
	_result->_structs->emplace_back(std::move(desc));
	(*(_result->_structs_name_to_idx))[struct_name] = _result->_structs->size() - 1;

	return true;
}

void DataLayoutStructureDescriptorParser::start_parse_single(DataLayoutStructureDescriptor* struct_ptr)
{
	DataPacketDataStructureElementDescriptor result{};
	MString struture_name = "";

	get_next_token();
	std::tie(result._element_type, struture_name) = parse_typename();

	get_next_token();
	result._arrays = parse_array_info();

	get_next_token();
	result._name = parse_element_name();

	get_next_token();
	result._desc = parse_desc_text();

	get_next_token();
	result._alignment = parse_alignment();

	get_next_token();
	if (_current_token == ";") {
		if (result._element_type == DataPacketMemberType::Structure) {
			auto idx = _result->_structs_name_to_idx->at(struture_name);
			struct_ptr->add_structure_member(_result->_structs->at(idx).get(), result._name, result._desc, result._arrays, result._alignment);
		} else {
			struct_ptr->add_normal_member(result);
		}
	} else {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: need ';' after statement. at line {}.", _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}
}

std::pair<DataPacketMemberType, MString> DataLayoutStructureDescriptorParser::parse_typename()
{
	if (_current_token_type == DescTextTokenType::Identifer) {
		DataPacketMemberType type = data_packet_basic_type_string_to_type(_current_token);

		if (type == DataPacketMemberType::Invaild) {

			if (_result->_structs_name_to_idx->contains(_current_token)) {
				return { DataPacketMemberType::Structure, _current_token };
			}

			MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: invaild typename {} ,at line {}.", _current_token.c_str(), _token_line);
			error(msg);
			throw Exception{ msg, 0 };
		}
		return { type, "" };
	} else {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: expected typename, but {}, at line {}.", _current_token.c_str(), _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}
}

MVector<uint32_t> DataLayoutStructureDescriptorParser::parse_array_info()
{
	if (_current_token != "[") {
		put_token_back();
		return {};
	}
	MVector<uint32_t> result = {};
	get_next_token();
	while (_current_token != "]") {
		if (_current_token_type == DescTextTokenType::Int) {
			result.push_back(std::stoi(_current_token));
		} else {
			MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: excepted array size, it's integer, but {}, at line {}.", _current_token.c_str(), _token_line);
			error(msg);
			throw Exception{ msg, 0 };
		}

		get_next_token();

		if (_current_token == ",") {
			get_next_token();
			continue;
		}
		if (_current_token != "]") {
			MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: excepted comma ',' or end ']', but {}, at line {}.", _current_token.c_str(), _token_line);
			error(msg);
			throw Exception{ msg, 0 };
		}
	}
	return result;
}

MString DataLayoutStructureDescriptorParser::parse_element_name()
{
	if (_current_token_type != DescTextTokenType::Identifer) {
		MString msg = MString::format("DataPacketDataStructreDescriptorText::parse code: expected element name, but {}, at line {}.", _current_token.c_str(), _token_line);
		error(msg);
		throw Exception{ msg, 0 };
	}
	return _current_token;
}

MString DataLayoutStructureDescriptorParser::parse_desc_text()
{
	if (_current_token_type != DescTextTokenType::String) {
		put_token_back();
		return "";
	}
	return _current_token;
}

uint16_t DataLayoutStructureDescriptorParser::parse_alignment()
{
	if (_current_token_type != DescTextTokenType::Int) {
		put_token_back();
		return 1;
	}
	return std::stoi(_current_token);
}
