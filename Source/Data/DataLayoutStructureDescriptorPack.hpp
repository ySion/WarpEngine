#pragma once

#include "Core/Core.hpp"
#include "DataLayoutStructureDescriptor.hpp"
#include "DataLayoutStructureDescriptorParser.hpp"

WARP_TYPE_NAME_2(Data, DataLayoutStructureDescriptorPack);

namespace Warp::Data {

	class DataLayoutStructureDescriptorPack : public Inherit<DataLayoutStructureDescriptorPack, Object> {
	public:
		DataLayoutStructureDescriptorPack();

		~DataLayoutStructureDescriptorPack() override = default;

		bool parse_and_replace_all(const MString& code_text);

		DataLayoutStructureDescriptor* find_struct(const MString& struct_name) const;

	private:
		std::unique_ptr<DataLayoutStructureDescriptorParser> _parser{};
		std::unique_ptr<DataLayoutStructureDescriptorGroup> _desc_infos{};
	};
};