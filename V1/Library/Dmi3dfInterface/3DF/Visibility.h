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

		void Set(VisibilityKit const & cInThat);
		VisibilityKit const & operator = (VisibilityKit const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::VisibilityKit; };

		//== Set Visibility Kit ========================================================================
		VisibilityKit & SetWindows(bool bInValue);
		VisibilityKit & SetEdges(bool bInValue);
		VisibilityKit & SetFaces(bool bInValue);
		VisibilityKit & SetLights(bool bInValue);
		VisibilityKit & SetLines(bool bInValue);
		VisibilityKit & SetMarkers(bool bInValue);
		VisibilityKit & SetVertices(bool bInValue);
		VisibilityKit & SetText(bool bInValue);
		VisibilityKit & SetGeometry(bool bInValue);
		VisibilityKit & SetEverything(bool bInValue);

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

		void Set(VisibilityControl const & cInThat);
		VisibilityControl & operator = (VisibilityControl const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::VisibilityControl; };

		//== Set Selectability Control =================================================================
		VisibilityControl & SetCuttingSections(bool bInValue);
		VisibilityControl & SetWindows(bool bInValue);
		VisibilityControl & SetEdges(bool bInValue);
		VisibilityControl & SetFaces(bool bInValue);
		VisibilityControl & SetLights(bool bInValue);
		VisibilityControl & SetLines(bool bInValue);
		VisibilityControl & SetMarkers(bool bInValue);
		VisibilityControl & SetVertices(bool bInValue);
		VisibilityControl & SetText(bool bInValue);
		VisibilityControl & SetGeometry(bool bInValue);
		VisibilityControl & SetEverything(bool bInValue);

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
		VisibilityControl & UnsetGeometry();
		VisibilityControl & UnsetEverything();

		bool ShowFaces(bool & bOutState) const;
	};
}