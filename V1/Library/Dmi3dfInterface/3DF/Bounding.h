#pragma once

#include "Kit.h"
#include "Control.h"

#include "Math.h"

namespace H3DF
{
	class API_3DF BoundingKit : public H3DF::Kit
	{
	public:
		BoundingKit();
		BoundingKit(BoundingKit const & cInThat);

		void Set(BoundingKit const & cInThat);
		BoundingKit & operator = (BoundingKit const & cInThat);

		bool Empty() const;

		bool Equals(BoundingKit const & cInKit) const;
		bool operator == (BoundingKit const & cInKit) const;
		bool operator != (BoundingKit const & cInKit) const;

		BoundingKit & SetVolume(SimpleSphere const & cInSphere);
		BoundingKit & SetVolume(SimpleCuboid const & cInCuboid);
		BoundingKit & SetExclusion(bool bInExclude);

		BoundingKit & UnsetVolume();
		BoundingKit & UnsetExclusion();
		BoundingKit & UnsetEverything();

		bool ShowVolume(SimpleSphere & cOutSphere, SimpleCuboid & cOutCuboid) const;

		bool ShowExclusion(bool & bOutEexclusion) const;
	};

	class API_3DF BoundingControl : public Control {
	public:
		explicit BoundingControl(SegmentKey & cInSegmentKey);
		BoundingControl(BoundingControl const & cInThat);

		void Set(BoundingControl const & cInThat);
		BoundingControl & operator = (BoundingControl const & cInThat);

		BoundingControl & SetExclusion(bool bInExclusion);

		BoundingControl & UnsetExclusion();

		bool ShowExclusion(bool & bOutExclusion) const;
	};
}