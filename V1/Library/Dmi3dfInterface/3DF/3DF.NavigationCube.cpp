#include "StdAfx.h"
#include "3DF.NavigationCube.h"
#include "3DF.Painter.h"

#include "3DF.KeyPath.h"
#include "3DF.Window.h"

#include "3DF.Selection.h"
#include "./Private/3DF.SelectionPrivate.h"

USING_3DF_NAMESPACE

namespace NavigationCubePreset
{
	class Format
	{
	public:

		Format(const char* format, ...)
		{
			va_list ap;

			va_start(ap, format);
			buffer.FormatV(format, ap);
			va_end(ap);
		}

		operator char const* () const
		{
			return buffer;
		}

	private:

		CStringA buffer;
	};



	COLORREF EdgeColor()
	{
		return 0;
	}

	COLORREF FaceColor()
	{
		return RGB(0xC0, 0xC0, 0xC0);
	}

	COLORREF LineColor()
	{
		//return RGB(0x00, 0x5E, 0x97);
		return RGB(0x80, 0x80, 0x80);
	}

	COLORREF TextColor()
	{
		return RGB(0x33, 0x33, 0x33);
	}



	double PlaneUnit()
	{
		return 0.4;
	}

	double EdgeUnit()
	{
		return 0.25;
	}

	double CornerUnit()
	{
		return 0.15;
	}

	double AxisUnit()
	{
		return 0.6;
	}
}

#define PRESET NavigationCubePreset



NavigationCube::NavigationCube(TDF::BaseView * view, WindowKey * pcInWindow) :
	m_pView(view),
	m_pcWindow(pcInWindow)
{
}

NavigationCube::~NavigationCube()
{
}

int NavigationCube::LButtonUp(HEventInfo & cInEvent)
{
	WindowPoint cPoint(cInEvent.GetMouseWindowPos());

	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Segment).SetRelatedLimit(0);//.SetProximity(0.001);// SetSorting(Selection::Sorting::ZSorting);
	
	SelectionResults cSelection;
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cSelectOption, cSelection);

	if (0 == cSelection.GetCount()) {
		return HLISTENER_PASS_EVENT;
	}

	SegmentKey cSelectKey;
	if (false == cSelection.Front()->ShowSelectedItem(cSelectKey)) {
		return HLISTENER_PASS_EVENT;
	}

	CString strName = cSelectKey.Name();
	TRACE(L"%s\n", strName);

	m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
	m_cOldHighlightSelection.Reset();

	for (int nIndex = 0; nIndex < (int)TDF::ViewMode::Count; nIndex++) {
		if (m_cSegments[nIndex] == cSelectKey) {
			m_pView->SetViewMode((TDF::ViewMode)nIndex);
			break;
		}
	}

	return HLISTENER_CONSUME_EVENT;
}

int NavigationCube::NoButtonDownAndMove(HEventInfo & cInEvent)
{
	char chPathName[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Segment(m_pView->GetSceneKey(), chPathName);

	WindowPoint cPoint(cInEvent.GetMouseWindowPos());

	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Segment).SetRelatedLimit(0);//.SetProximity(0.001);// SetSorting(Selection::Sorting::ZSorting);

	int nEvent = HLISTENER_PASS_EVENT;
	bool bUpdateFlag = false;

	SelectionResults cSelection;
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cSelectOption, cSelection);

	// 선택된 요소가 없은 경우
	if (0 == nSelectedCount) {
		// 기존에 선택된 요소가 있는 경우 처리
		if (0 < m_cOldHighlightSelection.GetCount()) {
			m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
			m_cOldHighlightSelection.Reset();
			bUpdateFlag = true;
		}
	}
	else {
		// 이전에 선택된것과 다른 경우
		if (m_cOldHighlightSelection != cSelection) {
			m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
			
			SegmentKey cSelectKey;
			cSelection.Front()->ShowSelectedItem(cSelectKey);

			bool bFindFlag = false;

			for (int nIndex = 0; nIndex < (int)TDF::ViewMode::Count; nIndex++) {
				if (m_cSegments[nIndex] == cSelectKey) {
					bFindFlag = true;
				}
			}

			if (true == bFindFlag) {
				HighlightOptionsKit cHighlightOptions;
				m_pcWindow->GetHighlightControl().Highlight(cSelection, cHighlightOptions, true);
				m_cOldHighlightSelection = cSelection;
			}
			else {
				m_cOldHighlightSelection.Reset();
			}

			bUpdateFlag = true;
			nEvent = HLISTENER_CONSUME_EVENT;
		}
	}

	if (true == bUpdateFlag) {
		m_pcWindow->Update();
	}

	return nEvent;
}

void NavigationCube::SetSize(ESize size)
{
	m_eCubeSize = size;
}



void NavigationCube::SetView(TDF::BaseView * view, WindowKey * pcInWindow) {
	m_pView = view;
	m_pcWindow = pcInWindow;
}



void NavigationCube::SetVisible(bool axis, bool cube)
{
	m_bAxisVisible = axis;
	m_bCubeVisible = cube;
}



bool NavigationCube::IsValid()
{
	return m_cubeSegment != HC_ERROR_KEY;
}


void NavigationCube::Create(float width, float height, HC_KEY parent)
{
	m_parentSegment = parent;
	ASSERT(m_parentSegment != HC_ERROR_KEY);

	HC_Open_Segment_By_Key(m_parentSegment);
	{
		OpenCubeSegment();
		{
			HC_Set_Selectability("everything = off");

			HC_Set_Rendering_Options("attribute lock = visibility"
				", hidden line removal options = render faces"
				", no lod"
				", no frame buffer effects"
				", no display lists"
				", depth range = (0, 0.1)"
				", simple shadow = off"
				", simple reflection = off"
				", no force grayscale"
				", diffuse color tint = off"
				", anti-alias = (text = on)"
			);

			SetWindowSize(width, height, false);

			HC_Set_Camera_Projection("orthographic");
			HC_Set_Visibility("cutting planes = off"
				", edges = off"
				", faces = on"
				", lights = off"
				", lines = on"
				", markers = off"
				", shadows = off"
				", text = on"
				", vertices = off"
			);

			HC_Set_Text_Font("name = franklin gothic book"
				", bold = on"
			);
			HC_Set_Text_Alignment("**");

			Painter::Segment::SetColor("faces", PRESET::FaceColor());
			Painter::Segment::SetColor("edges", PRESET::EdgeColor());
			Painter::Segment::SetColor("lines", PRESET::LineColor());
			Painter::Segment::SetColor("text", PRESET::TextColor());

			if (m_bCubeVisible) {
				CreateCube();
			}

			if (m_bAxisVisible) {
				CreateAxis();
			}
		}
		CloseCubeSegment();
	}
	HC_Close_Segment();
}



HC_KEY NavigationCube::HitTest(float x, float y, float z)
{
	ASSERT(FALSE);
	return HC_ERROR_KEY;
}



void NavigationCube::Transform()
{
	HPoint position, target, up_vector;

	HC_Open_Segment_By_Key(m_pView->GetSceneKey()); {
		HC_Show_Net_Camera_Target(&target.x, &target.y, &target.z);
		HC_Show_Net_Camera_Up_Vector(&up_vector.x, &up_vector.y, &up_vector.z);
		HC_Show_Net_Camera_Position(&position.x, &position.y, &position.z);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(m_pView->GetSceneKey()); {
		HC_Open_Segment_By_Key(m_cubeSegment); {
			HPoint oldposition;
			HPoint old_up_vector;

			double new_position[] = { position.x - target.x, position.y - target.y, position.z - target.z };

			HC_DCompute_Normalized_Vector(new_position, new_position);
			new_position[0] *= 5.0; new_position[1] *= 5.0; new_position[2] *= 5.0;

			HC_Show_Net_Camera_Position(&oldposition.x, &oldposition.y, &oldposition.z);
			double const difference[] = { fabs(oldposition.x - new_position[0]), fabs(oldposition.y - new_position[1]), fabs(oldposition.z - new_position[2]) };

			HC_Show_Net_Camera_Up_Vector(&old_up_vector.x, &old_up_vector.y, &old_up_vector.z);
			double const difference2[] = { fabs(old_up_vector.x - up_vector.x), fabs(old_up_vector.y - up_vector.y), fabs(old_up_vector.z - up_vector.z) };

			//			 we only modify the axis display if there has been an actual change in the camera settings
			//			 we also have to consider the up vector!!!
			if (difference[0] + difference[1] + difference[2] > 0.01 || difference2[0] + difference2[1] + difference2[2] > 0.01)
			{
				HC_Set_Camera_Target(0.0f, 0.0f, 0.0f);
				HC_Set_Camera_Position(new_position[0], new_position[1], new_position[2]);
				HC_Set_Camera_Up_Vector(up_vector.x, up_vector.y, up_vector.z);
			}
		} HC_Close_Segment();
	} HC_Close_Segment();
}

void NavigationCube::Transform_ORG()
{
	HPoint position;
	HPoint target;
	HVector up;
	float width, height;
	char projection[MVO_BUFFER_SIZE];

	HC_Open_Segment_By_Key(m_pView->GetSceneKey());
	{
		HC_Show_Net_Camera(&position, &target, &up, &width, &height, projection);
	}
	HC_Close_Segment();

	OpenCubeSegment();
	{
		//:TODO - only rotation
		HC_Set_Camera(&position, &target, &up, 2, 2, projection);
	}
	CloseCubeSegment();
}



void NavigationCube::OnSize(float width, float height)
{
	SetWindowSize(width, height);
}



void NavigationCube::OpenCubeSegment()
{
	if (m_cubeSegment == HC_ERROR_KEY) {
		m_cubeSegment = HC_Open_Segment("cube_window");
	}
	else {
		HC_Open_Segment_By_Key(m_cubeSegment);
	}
}



void NavigationCube::OpenPlaneSegment()
{
	if (m_planeSegment == HC_ERROR_KEY) {
		m_planeSegment = HC_Open_Segment("plane window");
	}
	else {
		HC_Open_Segment_By_Key(m_planeSegment);
	}
}



void NavigationCube::CloseCubeSegment()
{
	HC_Close_Segment();
}



void NavigationCube::ClosePlaneSegment()
{
	HC_Close_Segment();
}



void NavigationCube::CreateAxis()
{
	double plane = PRESET::PlaneUnit();
	double axis = PRESET::AxisUnit() - 0.05;
	double text = PRESET::AxisUnit() + 0.1;

	CreateAxis("x", "X", HPoint(axis, -plane, -plane), HPoint(text, -plane, -plane), RGB(255, 0, 0));
	CreateAxis("y", "Y", HPoint(-plane, axis, -plane), HPoint(-plane, text, -plane), RGB(0, 255, 0));
	CreateAxis("z", "Z", HPoint(-plane, -plane, axis), HPoint(-plane, -plane, text), RGB(0, 0, 255));

	//CreateAxis("x", "X", HPoint(axis, -plane, -plane), HPoint(text, -plane, -plane), RGB(0xD4, 0x23, 0x14));
	//CreateAxis("y", "Y", HPoint(-plane, axis, -plane), HPoint(-plane, text, -plane), RGB(0x30, 0x90, 0x48));
	//CreateAxis("z", "Z", HPoint(-plane, -plane, axis), HPoint(-plane, -plane, text), RGB(0x00, 0x63, 0xB1));
}



void NavigationCube::CreateCube()
{
	double plane = PRESET::PlaneUnit();

	// Cube wire

	HPoint maxPoint(plane, plane, plane);
	HPoint minPoint(-plane, -plane, -plane);
	HUtility::InsertWireframeBox(&maxPoint, &minPoint);

	// Plane and text

	m_cSegments[(int)TDF::ViewMode::top]		= CreatePlaneShell("top", "TOP", { 0, 0, plane }, { 0, 0, 0 });
 	m_cSegments[(int)TDF::ViewMode::bottom]		= CreatePlaneShell("bottom", "BOTTOM", { 0, 0, -plane }, { 0, 180, 0 });
	m_cSegments[(int)TDF::ViewMode::front]		= CreatePlaneShell("front", "FRONT", { 0, -plane, 0 }, { 90, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::back]		= CreatePlaneShell("back", "BACK", { 0, plane, 0 }, { 90, 0, 180 });
	m_cSegments[(int)TDF::ViewMode::left]		= CreatePlaneShell("left", "LEFT", { -plane, 0, 0 }, { 90, 0, -90 });
	m_cSegments[(int)TDF::ViewMode::right]		= CreatePlaneShell("right", "RIGHT", { plane, 0, 0 }, { 90, 0, 90 });

	// Edges - n: negative, p: positive

	m_cSegments[(int)TDF::ViewMode::py_nz]		= CreateEdgeShell("py_nz", { 0, plane, -plane }, { 0, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::py_pz]		= CreateEdgeShell("py_pz", { 0, plane, plane }, { 90, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::ny_pz]		= CreateEdgeShell("ny_pz", { 0, -plane, plane }, { 180, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::ny_nz]		= CreateEdgeShell("ny_nz", { 0, -plane, -plane }, { 270, 0, 0 });

	m_cSegments[(int)TDF::ViewMode::nx_nz]		= CreateEdgeShell("nx_nz", { -plane, 0, -plane }, { 0, 0, 90 });
	m_cSegments[(int)TDF::ViewMode::nx_pz]		= CreateEdgeShell("nx_pz", { -plane, 0, plane }, { 90, 0, 90 });
	m_cSegments[(int)TDF::ViewMode::px_pz]		= CreateEdgeShell("px_pz", { plane, 0, plane }, { 180, 0, 90 });
	m_cSegments[(int)TDF::ViewMode::px_nz]		= CreateEdgeShell("px_nz", { plane, 0, -plane }, { 270, 0, 90 });

	m_cSegments[(int)TDF::ViewMode::nx_py]		= CreateEdgeShell("nx_py", { -plane, plane, 0 }, { 0, 90, 0 });
	m_cSegments[(int)TDF::ViewMode::px_py]		= CreateEdgeShell("px_py", { plane, plane, 0 }, { 90, 90, 0 });
	m_cSegments[(int)TDF::ViewMode::px_ny]		= CreateEdgeShell("px_ny", { plane, -plane, 0 }, { 180, 90, 0 });
	m_cSegments[(int)TDF::ViewMode::nx_ny]		= CreateEdgeShell("nx_ny", { -plane, -plane, 0 }, { 270, 90, 0 });

	// Corners - n: negative, p: positive

	m_cSegments[(int)TDF::ViewMode::nx_py_nz]	= CreateCornerShell("nx_py_nz", { -plane, plane, -plane }, { 0, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::nx_py_pz]	= CreateCornerShell("nx_py_pz", { -plane, plane, plane }, { 90, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::nx_ny_pz]	= CreateCornerShell("nx_ny_pz", { -plane, -plane, plane }, { 180, 0, 0 });
	m_cSegments[(int)TDF::ViewMode::nx_ny_nz]	= CreateCornerShell("nx_ny_nz", { -plane, -plane, -plane }, { 270, 0, 0 });

	m_cSegments[(int)TDF::ViewMode::px_py_pz]	= CreateCornerShell("px_py_pz", { plane, plane, plane }, { 0, 180, 0 });
	m_cSegments[(int)TDF::ViewMode::px_py_nz]	= CreateCornerShell("px_py_nz", { plane, plane, -plane }, { 90, 180, 0 });
	m_cSegments[(int)TDF::ViewMode::px_ny_nz]	= CreateCornerShell("px_ny_nz", { plane, -plane, -plane }, { 180, 180, 0 });
	m_cSegments[(int)TDF::ViewMode::px_ny_pz]	= CreateCornerShell("px_ny_pz", { plane, -plane, plane }, { 270, 180, 0 });
}



HC_KEY NavigationCube::CreatePlaneShell(const char* name, const char* text, Triple pos, Triple angle)
{
	double unit = PRESET::PlaneUnit() - PRESET::CornerUnit();
	HPoint p1(-unit, unit);
	HPoint p2(unit, -unit);

	HPoint points[4];
	points[0] = p1;
	points[1].Set(p1.x, p2.y);
	points[2].Set(p2.x, p2.y);
	points[3].Set(p2.x, p1.y);

	int faces[] = {
		4, 0, 1, 2, 3
	};

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		HC_Set_Text_Font("transforms = on");

		HC_KEY shellKey = HC_Insert_Shell(4, points, 5, faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_KEY textKey = HC_Insert_Text(0, 0, 0, text);
		ASSERT(textKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY NavigationCube::CreateEdgeShell(const char* name, Triple pos, Triple angle)
{
	double width = PRESET::EdgeUnit();
	double height = PRESET::CornerUnit();

	HPoint points[6];
	points[0].Set(-width,       0,      0);
	points[1].Set(-width, -height,      0);
	points[2].Set( width, -height,      0);
	points[3].Set( width,       0,      0);
	points[4].Set( width,       0, height);
	points[5].Set(-width,       0, height);

	int faces[] = {
		4, 0, 1, 2, 3,
		4, 3, 4, 5, 0
	};

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		//Segment::SetColor("faces", RGB(255, 0, 0), 0.5);

		HC_KEY shellKey = HC_Insert_Shell(6, points, 10, faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY NavigationCube::CreateCornerShell(const char* name, Triple pos, Triple angle)
{
	double unit = PRESET::CornerUnit();

	HPoint points[7];
	points[0].Set(   0,     0,    0);
	points[1].Set(   0, -unit,    0);
	points[2].Set(unit, -unit,    0);
	points[3].Set(unit,     0,    0);
	points[4].Set(unit,     0, unit);
	points[5].Set(   0,     0, unit);
	points[6].Set(   0, -unit, unit);

	int faces[] = {
		4, 0, 1, 2, 3,
		4, 3, 4, 5, 0,
		4, 5, 6, 1, 0
	};

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		//Segment::SetColor("faces", RGB(0, 0, 255), 0.5);

		HC_KEY shellKey = HC_Insert_Shell(7, points, 15, faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY NavigationCube::CreateAxis(const char* name, const char* text, HPoint axisEnd, HPoint textCenter, COLORREF rgb)
{
	double unit = PRESET::PlaneUnit();
	HPoint axisStart(-unit, -unit, -unit);

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		Painter::Segment::SetColor("faces", rgb);
		Painter::Segment::SetColor("text", rgb);

		HC_KEY key = HC_Insert_Cylinder(&axisStart, &axisEnd, 0.01, "none");
		ASSERT(key != HC_ERROR_KEY);

		HC_KEY textKey = HC_Insert_Text(textCenter.x, textCenter.y, textCenter.z, text);
		ASSERT(textKey != HC_ERROR_KEY);
	}
	HC_Close_Segment();

	return segKey;
}



void NavigationCube::SetWindowSize(double width, double height, bool openSegment)
{
	if (openSegment) {
		OpenCubeSegment();
	}

	double windowSize = (double)m_eCubeSize;
	double fontSize = 12 * (double)m_eCubeSize / (double)ESize::Midium;

	double left = 1.0 - 2.0 / width * windowSize;
	double bottom = 1.0 - 2.0 / height * windowSize;

	//HC_Set_Window(left, 1.0, bottom, 1.0);

 	//HC_Set_Window(0.8, 1.0, 0.7, 1.0);
// 	HC_Set_Window_Pattern("clear");

	//HC_Set_Driver_Options("border, control area");
// 	//HC_Set_Color("windows=light gray");
// 	//HC_Set_Window_Frame("single");
// 	HC_Set_Window_Pattern("::");
// 	HC_Set_Color("windows=purple,window constrast=yellow");


	HC_Set_Rendering_Options(PRESET::Format("screen range = (%.6f, 1, %.6f, 1)", left, bottom));
	HC_Set_Text_Font(PRESET::Format("size = %.3f px", fontSize));

	if (openSegment) {
		CloseCubeSegment();
	}
}

#undef PRESET
