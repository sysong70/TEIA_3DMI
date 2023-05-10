#pragma once

#include "3DF.h"
#include "HBaseView.h"

OPEN_3DF_NAMESPACE

class NavigationCube
{
public:

	// Cube pixel size
	enum ESize
	{
		Small = 100,
		Midium = 200,
		Big = 300,
	};

	NavigationCube(HBaseView* view = nullptr);

	~NavigationCube();

	void SetSize(ESize size);

	void SetView(HBaseView* view);

	void SetVisible(bool axis, bool cube);

	bool IsValid();

public:

	void Create(float width, float height, HC_KEY parent);

	HC_KEY HitTest(float x, float y, float z);

	void Transform();
	void Transform_ORG();

	void OnSize(float width, float height);

private:

	struct Triple
	{
		double x;
		double y;
		double z;
	};

	void OpenCubeSegment();

	void OpenPlaneSegment();

	void CloseCubeSegment();

	void ClosePlaneSegment();

	void CreateAxis();

	void CreateCube();

	HC_KEY CreatePlaneShell(const char* name, const char* text, Triple pos, Triple angle);

	HC_KEY CreateEdgeShell(const char* name, Triple pos, Triple angle);

	HC_KEY CreateCornerShell(const char* name, Triple pos, Triple angle);

	HC_KEY CreateAxis(const char* name, const char* text, HPoint axisEnd, HPoint textCenter, COLORREF rgb);

	void SetWindowSize(double width, double height, bool openSegment = true);

	HBaseView* m_pView = nullptr;
	ESize m_eCubeSize = ESize::Midium;
	bool m_bAxisVisible = true;
	bool m_bCubeVisible = true;

	HC_KEY m_parentSegment = HC_ERROR_KEY;
	HC_KEY m_cubeSegment = HC_ERROR_KEY;
	HC_KEY m_planeSegment = HC_ERROR_KEY;
};

CLOSE_3DF_NAMESPACE
