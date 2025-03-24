#pragma once

#include <Json.h>

#include "../3DF/3DF.h"

#include "../3DF/Facility.AppOptions.h"
#include "../3DF/Window.h"

#include "Sprocket.h"

#include "../3DF.Signal.h"
#include "../Signal/Signal.h"

namespace H3DF
{
	class API_3DF Model : public Sprocket
	{
	public:
		Model();

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		PortfolioKey GetPortfolioKey();
		PortfolioKey const GetPortfolioKey() const;

		void SetBRepGeometry(bool brep);

		H3DF::ModelHandedness GetModelHandedness();
		void UpdateModelHandedness();

		SegmentKey & ModelsRoot() const;
		SegmentKey & MeasurementsRoot() const;
		SegmentKey & MarkupsRoot() const;

		SegmentKey & IncludeModel() const;
		SegmentKey & IncludeStyles() const;
	};
}