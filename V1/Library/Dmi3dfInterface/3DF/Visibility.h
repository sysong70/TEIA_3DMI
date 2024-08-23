#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class VisibilityKit : public Kit
	{
	public:
		enum class VisibilityType
		{
			Windows,
			Edges,
			Faces,
			Lights,
			Lines,
			Markers,
			Vertices,
			Text,
			Geometry,
			Everything,
			Count
		};

		VisibilityKit();
		VisibilityKit(VisibilityKit const & cInThat);

		H3DF::Type ObjectType() const override { return H3DF::Type::VisibilityKit; }

		void Set(VisibilityKit const & cInThat);
		VisibilityKit const & operator = (VisibilityKit const & cInThat);

		//== Set Visibility Kit ========================================================================
		VisibilityKit & SetWindows(bool bInState);
		VisibilityKit & SetEdges(bool bInState);
		VisibilityKit & SetFaces(bool bInState);
		VisibilityKit & SetLights(bool bInState);
		VisibilityKit & SetLines(bool bInState);
		VisibilityKit & SetMarkers(bool bInState);
		VisibilityKit & SetVertices(bool bInState);
		VisibilityKit & SetText(bool bInState);
		VisibilityKit & SetGeometry(bool bInState);
		VisibilityKit & SetEverything(bool bInState);

		//== Unset Visibility Kit ======================================================================
		VisibilityKit & UnsetWindows();
		VisibilityKit & UnsetEdges();
		VisibilityKit & UnsetFaces();
		VisibilityKit & UnsetLights();
		VisibilityKit & UnsetLines();
		VisibilityKit & UnsetMarkers();
		VisibilityKit & UnsetVertices();
		VisibilityKit & UnsetText();
		VisibilityKit & UnsetGeometry();
		VisibilityKit & UnsetEverything();
	};

	class API_3DF VisibilityControl : public Control
	{
	public:
		explicit VisibilityControl(SegmentKey & cInSegmentKey);
		VisibilityControl(VisibilityControl const & cInThat);

		H3DF::Type ObjectType() const override { return H3DF::Type::VisibilityControl; }

		void Set(VisibilityControl const & cInThat);
		VisibilityControl & operator = (VisibilityControl const & cInThat);

		//== Set Selectability Control =================================================================
		VisibilityControl & SetCuttingSections(bool bInState);
		VisibilityControl & SetWindows(bool bInState);
		VisibilityControl & SetEdges(bool bInState);
		VisibilityControl & SetFaces(bool bInState);
		VisibilityControl & SetLights(bool bInState);
		VisibilityControl & SetLines(bool bInState);
		VisibilityControl & SetMarkers(bool bInState);
		VisibilityControl & SetVertices(bool bInState);
		VisibilityControl & SetText(bool bInState);
		VisibilityControl & SetShadows(bool bInState);
		VisibilityControl & SetGeometry(bool bInState);
		VisibilityControl & SetEverything(bool bInState);

		//== Unset Selectability Control ===============================================================
		VisibilityControl & UnsetCuttingSections();
		VisibilityControl & UnsetWindows();
		VisibilityControl & UnsetEdges();
		VisibilityControl & UnsetFaces();
		VisibilityControl & UnsetLights();
		VisibilityControl & UnsetLines();
		VisibilityControl & UnsetMarkers();
		VisibilityControl & UnsetVertices();
		VisibilityControl & UnsetText();
		VisibilityControl & UnsetShadows();
		VisibilityControl & UnsetGeometry();
		VisibilityControl & UnsetEverything();

		bool ShowFaces(bool & bOutState) const;
	};
}