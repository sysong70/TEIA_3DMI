#pragma once

#include "Json.h"

#include "3DF.Facility.Base.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	class Preference : public Base
	{
	public:

		Preference();

		~Preference() override;

	public:

		Json::Object * Get() override;

		bool Set(Json::Object * pData) override;

		struct SELECT
		{
/*
			bool Solids = true;
			bool Surfaces = true;
			bool Wireframes = true;
			bool Attributes = true;
			bool HiddenObjects = true;
			bool References = true;
			bool ActiveFilter = true;
			bool SewModel = true;
			double SewingTolerance = true;
			bool ShellOrientation = true;
			ELoadingMode LoadingMode = ELoadingMode::Complete;

			Json::Object * Get();

			bool Set(Json::Object * pData);
*/
		}
		Select;

		struct TRANSPARENCY
		{
			CString Style = L"blended";
			CString Sorting = L"depth peeling";
			CString DepthPeelingLayers = L"3";
			bool PixelOIT = false;
			bool DepthWriting = false;

			Json::Object * Get();

			bool Set(Json::Object * pData);
		}
		Transparency;
	};
}

CLOSE_3DF_NAMESPACE