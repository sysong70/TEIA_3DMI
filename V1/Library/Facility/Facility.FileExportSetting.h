#pragma once

#include "Base.h"



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
