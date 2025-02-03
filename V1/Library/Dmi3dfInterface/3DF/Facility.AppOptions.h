#pragma once

#include "Facility.KernelOption.h"
#include "Facility.Preference.h"
#include "Facility.ImportOption.h"

namespace H3DF
{
	namespace Facility
	{
		class API_3DF AppOptions : public Base
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

extern H3DF::Facility::AppOptions TheAppOptions;
