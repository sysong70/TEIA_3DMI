#pragma once

#include "Session.Facility.KernelOption.h"
#include "Session.Facility.Preference.h"
#include "Session.Facility.ImportOption.h"

namespace SESSION
{
	namespace Facility
	{
		class AppOptions : public Base
		{
		public:

			AppOptions() {}
			~AppOptions() override {}

		public:

			Json::Object * Get() override {
				return nullptr;
			}
			bool Set(Json::Object * pData) override {
				return false;
			}

			KernelOption Kernel;
			KernelPreset Preset; // TODO - remove later
			Preference Preference;
			//ImportOption Import;
		};
	}
}

extern SESSION::Facility::AppOptions TheAppOptions;
