#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class VisibilityKit
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

	const bool * GetSelectabilityFlag() const { return m_bVisibilityFlag; }

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

	//== Operator ==================================================================================
	VisibilityKit & operator = (VisibilityKit const & cInKit);
	

private:
	bool m_bVisibilityFlag[(int) VisibilityType::Count];

	VisibilityKit & SetVisibility(VisibilityType eType, bool bInFlag);
};

class API_3DF VisibilityControl
{
public:
	VisibilityControl(SegmentKey & cInSegmentKey);

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
	
private:
	SegmentKey & m_cInSegmentKey;
	
	VisibilityControl & SetVisibility(CString strInType, bool bInValue);
	VisibilityControl & UnSetVisibility(CString strInType);
};

CLOSE_3DF_NAMESPACE