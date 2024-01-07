#pragma once

#include "MaterialParameter.hpp"

#include "Vk/Context.hpp"

WARP_TYPE_NAME_2(Gpu, MaterialSource);

namespace Warp::Gpu {

	class MaterialSource : public Inherit<MaterialSource, Object> {
	public:
		MaterialSource(Context* ctx) : _ctx(ctx) {}

		~MaterialSource() override = default;

		bool compile();

		inline Context* get_context() const noexcept { return _ctx; }

		inline ShaderModule* get_shader() const noexcept { return _shader.get(); }

		void add_parameter(const MaterialParameter& param) noexcept;

		void remove_parameter(std::string_view name, std::string_view identifier, MaterialParamterType type) noexcept;

	private:
		Context* _ctx{};

		MMap<MString, uint8_t> _param_identifer_to_index{};
		MMap<MString, uint8_t> _param_name_to_index{};

		std::vector<MaterialParameter> _param_infos{};

		std::unique_ptr<ShaderModule> _shader;
	};
}