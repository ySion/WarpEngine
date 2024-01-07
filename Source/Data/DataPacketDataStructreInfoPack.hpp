#pragma once
#include "Core/Inherit.hpp"

#include "DataPacketDataStructureDescriptorParser.hpp"

WARP_TYPE_NAME_2(Data, DataPacketDataStructreInfoPack);

namespace Warp::Data {
	class DataPacketDataStructreInfoPack : public Inherit<DataPacketDataStructreInfoPack, Object> {
	public:
		DataPacketDataStructreInfoPack();

		~DataPacketDataStructreInfoPack() override = default;

		bool parse_and_replace_all(const MString& code_text);

		DataPacketDataStructureDescriptor* find_struct(const MString& struct_name) const;

	private:
		std::unique_ptr<DataPacketDataStructreDescriptorParser> _parser{};
		std::unique_ptr<DataPacketDataStructreDescriptorInfo> _desc_infos{};
	};
};