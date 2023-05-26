#pragma once

#include "3DF.Facility.Base.h"

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

		struct SESSION
		{
			double DpiScale = 1.0;
		}
		Session;

		struct GENERAL
		{
			const char* FontName();
			double FontSize = 9;
			ELanguage Language = ELanguage::English;
			//ELanguage Language = ELanguage::Korean;

			CString Local(CString source);
		}
		General;

		struct MOUSE
		{
			bool SwapLeftRight = false;
			bool SwapWheelDirection = false;
		}
		Mouse;

		struct NAVCUBE
		{
			const char* FontName = "franklin gothic book";
			double FontSize = 9.5;
			bool ShowAxis = true;
			bool ShowCube = true;
		}
		NavCube;
	};
}

CLOSE_3DF_NAMESPACE
