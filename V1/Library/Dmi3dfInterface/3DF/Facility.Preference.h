#pragma once

#include "Facility.Base.h"

namespace H3DF
{
	namespace Facility
	{
		class API_3DF Preference : public Base
		{
		public:

			Preference() {}
			~Preference() override {}

		public:

			Json::Object * Get() override;

			bool Set(Json::Object * pData) override;

		public:

			struct SESSION
			{
				double DpiScale = 1.0;
			}
			Session;

			struct ENVIRONMENT
			{
				struct API_3DF GENERAL
				{
					const char * FontName();
					double FontSize = 9;
					ELanguage Language = ELanguage::English;

					CString Local(CString source);

					bool Set(Json::Object * pData);
				}
				General;

				struct MOUSE
				{
					bool SwapPanAndRotate = false;
					bool ReverseWheelDirection = false;

					bool Set(Json::Object * pData);
				}
				Mouse;

				bool Set(Json::Object * pData);
			}
			Environment;

			struct VIEWS
			{
				struct COLORS
				{
					COLORREF BackTop = RGB(0x30, 0x30, 0x30);
					COLORREF BackBottom = RGB(0x30, 0x30, 0x30);

					bool Set(Json::Object * pData);
				}
				Colors;

				struct NAVIGATION_CUBE
				{
					const char * FontName = "franklin gothic book";
					double FontSize = 5.0;
					int Size = 200;
					bool ShowAxis = true;
					bool ShowCube = true;

					bool Set(Json::Object * pData);
				}
				NavCube;

				bool Set(Json::Object * pData);
			}
			Views;
		};
	}
}