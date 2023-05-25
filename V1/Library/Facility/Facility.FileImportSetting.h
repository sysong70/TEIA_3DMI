#pragma once

#include "Base.h"



namespace Facility
{
	class FileImportSetting : public Base
	{
	public:

		FileImportSetting() {}
		~FileImportSetting() override {}

	public:

		Json::Object* Get() override {
			return nullptr;
		}
		bool Set(Json::Object* pcData) override {
			return false;
		}
	};
}
