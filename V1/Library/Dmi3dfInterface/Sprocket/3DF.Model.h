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

		void SetBRepGeometry(bool brep);

		H3DF::ModelHandedness GetModelHandedness();
		void UpdateModelHandedness();

		SegmentKey & Models() const;
		SegmentKey & Measurements() const;
		SegmentKey & Markups() const;

		SegmentKey & IncludeModel() const;
		SegmentKey & IncludeStyles() const;
	};
}