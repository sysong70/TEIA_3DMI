#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class SelectabilityKit
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
		Everything,
		Count
	};

	SelectabilityKit();

	const bool * GetSelectabilityFlag() const { return m_bSelectabilityFlag; }

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

	//== Operator ==================================================================================
	SelectabilityKit & operator = (SelectabilityKit const & cInKit);
	

private:
	bool m_bSelectabilityFlag[(int) SelectabilityType::Count];

	SelectabilityKit & SetSelectability(SelectabilityType eType, bool bInFlag);
};

class API_3DF SelectabilityControl
{
public:
	SelectabilityControl(SegmentKey & cInSegmentKey);

	//== Set Selectability Control =================================================================
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

	//== Unset Selectability Control ===============================================================
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
	SegmentKey & m_cInSegmentKey;
	
	SelectabilityControl & SetSelectability(CString strInType, bool bInValue);
	SelectabilityControl & UnSetSelectability(CString strInType);
};

CLOSE_3DF_NAMESPACE