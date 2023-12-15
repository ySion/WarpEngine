#include "iostream"

#include "Core/Object.hpp"
#include "Core/own_ptr.hpp"
#include "Core/observer_ptr.hpp"
#include "Core/Inherit.hpp"
#include "Core/Allocator.hpp"
#include "Core/TypeName.hpp"
#include "Core/MIStl.h"
#include "Vk/Instance.hpp"

using namespace Warp;


namespace Warp
{
	uint64_t p = 0;
	class APP;
	WARP_TYPE_NAME(APP)

	class APP : public Inherit<APP, Object>{
	public:
		APP() {
			p++;
		}

		void pr() {
			printf("hello worldf");
		}
	};


	class APP2 : public Inherit<APP2, APP> {
	public:
		APP2() {
			p++;
		}
	};

	class DD : public Inherit<DD, Object> {
	public:
		DD() = default;
		DD(int i) {
			p++;
		}
	};
}




int main() {


	{
		own_ptr<Gpu::Instance> ptt3 = Gpu::Instance::create();

		//own_ptr<Gpu::Instance> ptt3 = Gpu::Instance::create();

		//MVector<own_ptr<DD>> P{};

		observer_ptr<Gpu::Instance> ins = ptt3.get();

	}


	MString hello = "hello world";

	auto a = hello.split(" ");


	MString t2 = MString::format("hello world {}, {}, {}", 666, 222, "kakaka");

	std::cout << t2 << std::endl;

	{
		for(int i = 0; i < 100000; i++) {
			auto p = Warp::APP::create();
			p.release();
			
		}
	}
	
	observer_ptr<APP> ptt = nullptr;
	observer_ptr<DD> ptt2 = nullptr;



	if (ptt.is_object_vaild()) {
		printf("vaild\n");
	} else {
		printf("not vaild\n");
	}
	{
		own_ptr<APP> p = APP::create();
		own_ptr<DD> p2 = DD::create();

		ptt = p.get();
		ptt2 = p2.get();
		
		printf("\n%d\n", ptt2->is_compatible(ptt->get_type_info()));

		if (ptt.is_object_vaild())
		{
			printf("vaild\n");
		} else {
			printf("not vaild\n");
		}
	}
	if (ptt.is_object_vaild()) {
		printf("vaild\n");
	} else {
		printf("not vaild\n");
	}

	
	printf("=====================\n");
	auto b = AllocatorGroup::get_instance()->get_all_allocators();
	for(const auto& [name, ptr] : b) {
		std::cout << std::format("Object Manager: type \"{}\" at address {}, have {} \n", name, (void*)ptr, ptr->get_object_count());
	}
	printf("=====================\n");

	return 0;
}