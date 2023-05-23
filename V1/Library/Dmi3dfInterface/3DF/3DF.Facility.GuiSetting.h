#pragma once

#include "3DF.Facility.Base.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	enum class ELanguage
	{
		English = 0,
		Korean,
	};



	class GuiSetting : public Base
	{
	public:

		GuiSetting() {}
		~GuiSetting() override {}

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
	};
}

CLOSE_3DF_NAMESPACE
