#pragma once

#include "3DF.Facility.Base.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	class FileExportSetting : public Base
	{
	public:

		FileExportSetting() {}
		~FileExportSetting() override {}

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
