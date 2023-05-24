#pragma once

#include "3DF.Facility.Base.h"

OPEN_3DF_NAMESPACE

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

CLOSE_3DF_NAMESPACE
