#include "DataPacketDataStructreInfoPack.hpp"

using namespace Warp::Data;

DataPacketDataStructreInfoPack::DataPacketDataStructreInfoPack()
{
	_parser = std::make_unique<DataPacketDataStructreDescriptorParser>();
}

bool DataPacketDataStructreInfoPack::parse_and_replace_all(const MString& code_text)
{

	_parser->set_desc_text(code_text);
	auto ptr = std::move(_parser->parse());

	if (ptr == nullptr) {
		return false;
	}

	_desc_infos = std::move(ptr);
	set_dirty();
	return true;
}

DataPacketDataStructureDescriptor* DataPacketDataStructreInfoPack::find_struct(
	const MString& struct_name) const
{
	if (!_desc_infos) return nullptr;
	if (_desc_infos->_structs_name_to_idx->contains(struct_name)) {
		const auto idx = _desc_infos->_structs_name_to_idx->at(struct_name);
		return _desc_infos->_structs->at(idx).get();
	}
	return nullptr;
}
