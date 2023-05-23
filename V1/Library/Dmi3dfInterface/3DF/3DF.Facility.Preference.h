#pragma once

#include "3DF.Facility.FileImportSetting.h"
#include "3DF.Facility.FileExportSetting.h"
#include "3DF.Facility.GuiSetting.h"
#include "3DF.Facility.KernelSetting.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	class Preference : public Base
	{
	public:

		Preference() {}
		~Preference() override {}

	public:

		Json::Object* Get() override {
			return nullptr;
		}
		bool Set(Json::Object* pcData) override {
			return false;
		}

		KernelSetting Kernel;
		AppSetting App; //:TODO - remove later
		GuiSetting Gui;
		FileImportSetting Import;
		FileExportSetting Export;
	};
}

CLOSE_3DF_NAMESPACE

extern TDF::Facility::Preference ThePreference;
