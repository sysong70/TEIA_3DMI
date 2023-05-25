#pragma once

#include "Base.h"



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
			// 100 ~ 500, interval 100
			int CubeSize = 200;
			bool ShowAxis = true;
			bool ShowCube = true;
		}
		NavCube;
	};
}
