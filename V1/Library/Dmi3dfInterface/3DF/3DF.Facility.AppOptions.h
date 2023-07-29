#pragma once

#include "3DF.Facility.KernelOption.h"
#include "3DF.Facility.Preference.h"
#include "3DF.Facility.ImportOption.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	class AppOptions : public Base
	{
	public:

		AppOptions() {}
		~AppOptions() override {}

	public:

		Json::Object* Get() override {
			return nullptr;
		}
		bool Set(Json::Object* pData) override {
			return false;
		}

		KernelOption Kernel;
		KernelPreset Preset; //:TODO - remove later
		Preference Preference;
		//ImportOption Import;
	};
}

CLOSE_3DF_NAMESPACE

extern TDF::Facility::AppOptions TheAppOptions;
