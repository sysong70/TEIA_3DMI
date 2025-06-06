#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class SelectabilityKit : public Kit
	{
	public:
		enum class SelectabilityType
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
			Count,
			Everything
		};

		SelectabilityKit();
		SelectabilityKit(SelectabilityKit const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::SelectabilityKit;
		H3DF::Type ObjectType() const { return staticType; };

		static SelectabilityKit GetDefault();

		void Set(SelectabilityKit const & cInKit);
		void Show(SelectabilityKit & cOutKit) const;

		SelectabilityKit & operator = (SelectabilityKit const & cInKit);

		bool Empty() const;

		bool Equals(SelectabilityKit const & cInKit) const;
		bool operator == (SelectabilityKit const & cInKit) const;
		bool operator != (SelectabilityKit const & cInKit) const;

		//== Set Selectability Kit =====================================================================
		SelectabilityKit & SetWindows(bool bInValue);
		SelectabilityKit & SetEdges(bool bInValue);
		SelectabilityKit & SetFaces(bool bInValue);
		SelectabilityKit & SetLights(bool bInValue);
		SelectabilityKit & SetLines(bool bInValue);
		SelectabilityKit & SetMarkers(bool bInValue);
		SelectabilityKit & SetVertices(bool bInValue);
		SelectabilityKit & SetText(bool bInValue);
		SelectabilityKit & SetGeometry(bool bInValue);
		SelectabilityKit & SetEverything(bool bInValue);

		//== Unset Selectability Kit ===================================================================
		SelectabilityKit & UnsetWindows();
		SelectabilityKit & UnsetEdges();
		SelectabilityKit & UnsetFaces();
		SelectabilityKit & UnsetLights();
		SelectabilityKit & UnsetLines();
		SelectabilityKit & UnsetMarkers();
		SelectabilityKit & UnsetVertices();
		SelectabilityKit & UnsetText();
		SelectabilityKit & UnsetGeometry();
		SelectabilityKit & UnsetEverything();

		bool ShowWindows(Selectability::Value & cOuValue) const;
		bool ShowEdges(Selectability::Value & cOuValue) const;
		bool ShowFaces(Selectability::Value & cOuValue) const;
		bool ShowLights(Selectability::Value & cOuValue) const;
		bool ShowLines(Selectability::Value & cOuValue) const;
		bool ShowMarkers(Selectability::Value & cOuValue) const;
		bool ShowVertices(Selectability::Value & cOuValue) const;
		bool ShowText(Selectability::Value & cOuValue) const;
	};

	class API_3DF SelectabilityControl : public Control
	{
	public:
		explicit SelectabilityControl(SegmentKey & cInSegmentKey);
		SelectabilityControl(SelectabilityControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::SelectabilityControl;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(SelectabilityControl const & cInThat);
		SelectabilityControl & operator = (SelectabilityControl const & cInThat);

		//== Set Selectability Control =============================================================
		SelectabilityControl & SetWindows(bool bInValue);
		SelectabilityControl & SetEdges(bool bInValue);
		SelectabilityControl & SetFaces(bool bInValue);
		SelectabilityControl & SetLights(bool bInValue);
		SelectabilityControl & SetLines(bool bInValue);
		SelectabilityControl & SetMarkers(bool bInValue);
		SelectabilityControl & SetVertices(bool bInValue);
		SelectabilityControl & SetText(bool bInValue);
		SelectabilityControl & SetGeometry(bool bInValue);
		SelectabilityControl & SetEverything(bool bInValue);

		//== Unset Selectability Control ===========================================================
		SelectabilityControl & UnsetWindows();
		SelectabilityControl & UnsetEdges();
		SelectabilityControl & UnsetFaces();
		SelectabilityControl & UnsetLights();
		SelectabilityControl & UnsetLines();
		SelectabilityControl & UnsetMarkers();
		SelectabilityControl & UnsetVertices();
		SelectabilityControl & UnsetText();
		SelectabilityControl & UnsetGeometry();
		SelectabilityControl & UnsetEverything();

	private:
		SelectabilityControl() = default;
	};
}