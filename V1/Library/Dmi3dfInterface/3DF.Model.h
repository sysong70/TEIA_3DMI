#pragma once

#include <Json.h>

#include "3DF/3DF.h"

#include "3DF/Facility.AppOptions.h"
#include "3DF/Window.h"

#include "Sprocket.h"

#include "3DF.Signal.h"
#include "../Signal/Signal.h"

namespace H3DF
{
	class API_3DF Model : public Sprocket
	{
	public:
		Model();

		H3DF::Type ObjectType() const { return H3DF::Type::Model; };

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		void SetBRepGeometry(bool brep);

		H3DF::ModelHandedness GetModelHandedness();
		void UpdateModelHandedness();
	};
}