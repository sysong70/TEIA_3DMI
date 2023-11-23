#pragma once

#include "Facility.Base.h"

namespace H3DF
{
	namespace Facility
	{
		class API_3DF ImportOption : public Base
		{
		public:

			ImportOption(CString name = L"");

			~ImportOption() override;

		public:

			Json::Object * Get() override;

			bool Set(Json::Object * pData) override;

			enum class EReadingMode
			{
				FeatureTress,
				BRep,
				TessellationOnly,
			};

			enum class ELoadingMode
			{
				Complete,
				Incremental,
			};

			struct GENERAL
			{
				EReadingMode ReadingMode = EReadingMode::BRep;
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
			}
			General;

			enum class ETessLevel
			{
				ExtraLow,
				Low,
				Medium,
				High,
				ExtraHigh,
				Custom,
			};

			enum class EChordLimit
			{
				Ratio,
				Height,
			};

			struct TESSELLATION
			{
				ETessLevel TessLevel = ETessLevel::Medium;
				EChordLimit ChordLimitType = EChordLimit::Ratio;
				int ChordLimit = 2000;
				int AngleTolerance = 40;
				bool PreserveUV = true;
				int MaxEdgeLength = 0;
				bool AccurateTess = true;
				bool GridAlignedTess = true;
				double MaxStitchLength = 0.001;
				bool SurfaceCurvatures = true;

				Json::Object * Get();

				bool Set(Json::Object * pData);
			}
			Tessellation;
		};
	}
}