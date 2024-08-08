#pragma once

#include "Kit.h"
#include "Control.h"

#include "Math.h"

namespace H3DF
{
	class API_3DF Culling {
	public:
		enum class Face {
			Off, // No faces will be culled.
			Back, // Culls faces with normals that face away from the camera.
			Front, // Culls faces with normals that face towards the camera.
		};

	private:
		Culling() {}
	};

/*
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

*/
	class API_3DF CullingControl : public Control {
	public:
		explicit CullingControl(SegmentKey & cInSegmentKey);
		CullingControl(CullingControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::CullingControl;
		H3DF::Type ObjectType() const override { return staticType; };

		void Set(CullingControl const & cInThat);
		CullingControl & operator = (CullingControl const & cInThat);

		CullingControl & SetBackFace(bool bInState);
		CullingControl & SetFace(Culling::Face eInState);

	};
}