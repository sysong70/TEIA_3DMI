#include "StdAfx.h"
#include "NavigationCube.h"
#include "Painter.h"

#include "KeyPath.h"
#include "Window.h"

#include "Selection.h"
#include "Highlight.h"

#include "Facility.AppOptions.h"
#include "./Impl/Selection.Impl.h"

#include "./Impl/WindowImpl.h"

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
#define TheCube TheAppOptions.Preference.Views.NavCube


class NavigationCubeImpl : public Impl
{
public:

	void Copy(const NavigationCubeImpl * pcInThat) {
		m_bInitialized = pcInThat->m_bInitialized;

		m_pcBaseView = pcInThat->m_pcBaseView;

		m_windowSize = pcInThat->m_windowSize;

		m_parentSegment = pcInThat->m_parentSegment;
		m_cubeSegment = pcInThat->m_cubeSegment;

		for (int nIndex = 0; nIndex < (int)H3DF::ViewDirection::Mode::Count; nIndex++) {
			m_cSegments[nIndex] = pcInThat->m_cSegments[nIndex];
		}

		m_pcHighlightCtrl = pcInThat->m_pcHighlightCtrl;

		m_cOldHighlightSelection = pcInThat->m_cOldHighlightSelection;
	}
		
	bool m_bInitialized = false;

	BaseView * m_pcBaseView = nullptr;

	H3DF::Point2D m_windowSize;

	HC_KEY m_parentSegment = HC_ERROR_KEY;
	HC_KEY m_cubeSegment = HC_ERROR_KEY;

	SegmentKey m_cSegments[(int)H3DF::ViewDirection::Mode::Count];

	H3DF::HighlightControl * m_pcHighlightCtrl = nullptr;

	SelectionResults m_cOldHighlightSelection;
};



H3DF::NavigationCube::NavigationCube(H3DF::BaseView * view, WindowKey * pcInWindow)
{
	NavigationCubeImpl * pcImpl = new NavigationCubeImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->m_pcBaseView = view;
}

H3DF::NavigationCube::~NavigationCube()
{
}

void H3DF::NavigationCube::Set(NavigationCube const & cInThat)
{
	NavigationCubeImpl * pcImpl = (NavigationCubeImpl *)m_pcImpl;
	NavigationCubeImpl * pcInThatImpl = (NavigationCubeImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

NavigationCube const & H3DF::NavigationCube::operator = (NavigationCube const & cInThat)
{
	Set(cInThat);
	return *this;
}

int H3DF::NavigationCube::LButtonUp(Operator::Event & cInEvent, SelectionItem & cInItem)
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {  assert(false); }
/*
	WindowPoint cPoint(cInEvent.GetMouseWindowPos().x, cInEvent.GetMouseWindowPos().y, cInEvent.GetMouseWindowPos().z);

	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Segment).SetRelatedLimit(0);
	
	SelectionResults cSelection;
	size_t nSelectedCount = pcImpl->m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cSelectOption, cSelection);

	if (0 == cSelection.GetCount()) {
		return HLISTENER_PASS_EVENT;
	}

	SegmentKey cSelectKey;
	if (false == cSelection.Front().ShowSelectedItem(cSelectKey)) {
		return HLISTENER_PASS_EVENT;
	}

	CStringA strName = cSelectKey.Name();
	// TRACE(L"%s\n", strName);
*/

	SegmentKey cSelectKey;
	if (false == cInItem.ShowSelectedItem(cSelectKey)) {
		return HLISTENER_PASS_EVENT;
	}

	// CStringA strName = cSelectKey.Name();
	// TRACE(L"%s\n", strName);

	for (int nIndex = 0; nIndex < (int)H3DF::ViewDirection::Mode::Count; nIndex++) {
		if (pcImpl->m_cSegments[nIndex] == cSelectKey) {
			// 선택이 되었다면 Unhighlight하도록 한다.
			if (nullptr != pcImpl->m_pcHighlightCtrl) {
				pcImpl->m_pcHighlightCtrl->UnhighlightEverything();
			}

			pcImpl->m_pcBaseView->SetViewDirection((H3DF::ViewDirection::Mode)nIndex);

			return HLISTENER_CONSUME_EVENT;
		}
	}

	return HLISTENER_PASS_EVENT;
}



int H3DF::NavigationCube::LButtonDownAndMove(Operator::Event & cInEvent)
{
/*
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (0 < pcImpl->m_cOldHighlightSelection.GetCount()) {
		pcImpl->m_pcWindow->GetHighlightControl().Unhighlight(pcImpl->m_cOldHighlightSelection);
		pcImpl->m_cOldHighlightSelection.Reset();
	}
*/

	return HLISTENER_PASS_EVENT;
}



void H3DF::NavigationCube::SetView(H3DF::BaseView * pcInView) 
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcBaseView = pcInView;
}



bool H3DF::NavigationCube::IsValid()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cubeSegment != HC_ERROR_KEY;
}



bool H3DF::NavigationCube::IsInitialized()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_bInitialized;
}



void H3DF::NavigationCube::SetHighlightControl(H3DF::HighlightControl & cInHighlightCtrl)
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcHighlightCtrl = &cInHighlightCtrl;
}



void H3DF::NavigationCube::Create(float width, float height, HC_KEY parent)
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_windowSize.x = width;
	pcImpl->m_windowSize.y = height;
	pcImpl->m_parentSegment = parent;
	ASSERT(pcImpl->m_parentSegment != HC_ERROR_KEY);

	if (TheCube.ShowAxis == false && TheCube.ShowCube == false) {
		return;
	}

	HC_Open_Segment_By_Key(pcImpl->m_parentSegment);
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

			if (TheCube.ShowCube) {
				CreateCube();
			}

			if (TheCube.ShowAxis) {
				CreateAxis();
			}
		}
		CloseCubeSegment();
	}
	HC_Close_Segment();

	pcImpl->m_bInitialized = true;
}



void H3DF::NavigationCube::Recreate()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HC_Open_Segment_By_Key(pcImpl->m_parentSegment);
	HC_Delete_By_Key(pcImpl->m_cubeSegment);
	HC_Close_Segment();

	Create(pcImpl->m_windowSize.x, pcImpl->m_windowSize.y, pcImpl->m_parentSegment);
}



HC_KEY H3DF::NavigationCube::HitTest(float x, float y, float z)
{
	ASSERT(FALSE);
	return HC_ERROR_KEY;
}



void H3DF::NavigationCube::Transform()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HPoint position, target, up_vector;

	HC_Open_Segment_By_Key(pcImpl->m_pcBaseView->GetSceneKey()); {
		HC_Show_Net_Camera_Target(&target.x, &target.y, &target.z);
		HC_Show_Net_Camera_Up_Vector(&up_vector.x, &up_vector.y, &up_vector.z);
		HC_Show_Net_Camera_Position(&position.x, &position.y, &position.z);

		HC_Open_Segment_By_Key(pcImpl->m_cubeSegment); {
			HPoint oldposition;
			HPoint old_up_vector;

			double new_position[] = { position.x - target.x, position.y - target.y, position.z - target.z };

			HC_DCompute_Normalized_Vector(new_position, new_position);
			new_position[0] *= 5.0; new_position[1] *= 5.0; new_position[2] *= 5.0;

			HC_Show_Net_Camera_Position(&oldposition.x, &oldposition.y, &oldposition.z);
			double const difference[] = { fabs(oldposition.x - new_position[0]), fabs(oldposition.y - new_position[1]), fabs(oldposition.z - new_position[2]) };

			HC_Show_Net_Camera_Up_Vector(&old_up_vector.x, &old_up_vector.y, &old_up_vector.z);
			double const difference2[] = { fabs(old_up_vector.x - up_vector.x), fabs(old_up_vector.y - up_vector.y), fabs(old_up_vector.z - up_vector.z) };

			// we only modify the axis display if there has been an actual change in the camera settings
			// we also have to consider the up vector!!!
			if (difference[0] + difference[1] + difference[2] > 0.01 || difference2[0] + difference2[1] + difference2[2] > 0.01)
			{
				HC_Set_Camera_Target(0.0f, 0.0f, 0.0f);
				HC_Set_Camera_Position(new_position[0], new_position[1], new_position[2]);
				HC_Set_Camera_Up_Vector(up_vector.x, up_vector.y, up_vector.z);
			}
		} HC_Close_Segment();
	} HC_Close_Segment();
}



void H3DF::NavigationCube::OnSize(float width, float height)
{
	SetWindowSize(width, height);
}



void H3DF::NavigationCube::OpenCubeSegment()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_cubeSegment == HC_ERROR_KEY) {
		pcImpl->m_cubeSegment = HC_Open_Segment("cube_window");
	}
	else {
		HC_Open_Segment_By_Key(pcImpl->m_cubeSegment);
	}
}



void H3DF::NavigationCube::CloseCubeSegment()
{
	HC_Close_Segment();
}



void H3DF::NavigationCube::CreateAxis()
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



void H3DF::NavigationCube::CreateCube()
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	double unit = PRESET::PlaneUnit();

	CreateCubeWire();

	// Plane and text

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::top]		= CreatePlaneShell("top", "TOP", { 0, 0, unit }, { 0, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::bottom]		= CreatePlaneShell("bottom", "BOTTOM", { 0, 0, -unit }, { 0, 180, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::front]		= CreatePlaneShell("front", "FRONT", { 0, -unit, 0 }, { 90, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::back]		= CreatePlaneShell("back", "BACK", { 0, unit, 0 }, { 90, 0, 180 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::left]		= CreatePlaneShell("left", "LEFT", { -unit, 0, 0 }, { 90, 0, -90 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::right]		= CreatePlaneShell("right", "RIGHT", { unit, 0, 0 }, { 90, 0, 90 });

	// Edges - n: negative, p: positive

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::py_nz]		= CreateEdgeShell("py_nz", { 0, unit, -unit }, { 0, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::py_pz]		= CreateEdgeShell("py_pz", { 0, unit, unit }, { 90, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::ny_pz]		= CreateEdgeShell("ny_pz", { 0, -unit, unit }, { 180, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::ny_nz]		= CreateEdgeShell("ny_nz", { 0, -unit, -unit }, { 270, 0, 0 });

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_nz]		= CreateEdgeShell("nx_nz", { -unit, 0, -unit }, { 0, 0, 90 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_pz]		= CreateEdgeShell("nx_pz", { -unit, 0, unit }, { 90, 0, 90 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_pz]		= CreateEdgeShell("px_pz", { unit, 0, unit }, { 180, 0, 90 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_nz]		= CreateEdgeShell("px_nz", { unit, 0, -unit }, { 270, 0, 90 });

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_py]		= CreateEdgeShell("nx_py", { -unit, unit, 0 }, { 0, 90, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_py]		= CreateEdgeShell("px_py", { unit, unit, 0 }, { 90, 90, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_ny]		= CreateEdgeShell("px_ny", { unit, -unit, 0 }, { 180, 90, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_ny]		= CreateEdgeShell("nx_ny", { -unit, -unit, 0 }, { 270, 90, 0 });

	// Corners - n: negative, p: positive

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_py_nz]	= CreateCornerShell("nx_py_nz", { -unit, unit, -unit }, { 0, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_py_pz]	= CreateCornerShell("nx_py_pz", { -unit, unit, unit }, { 90, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_ny_pz]	= CreateCornerShell("nx_ny_pz", { -unit, -unit, unit }, { 180, 0, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::nx_ny_nz]	= CreateCornerShell("nx_ny_nz", { -unit, -unit, -unit }, { 270, 0, 0 });

	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_py_pz]	= CreateCornerShell("px_py_pz", { unit, unit, unit }, { 0, 180, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_py_nz]	= CreateCornerShell("px_py_nz", { unit, unit, -unit }, { 90, 180, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_ny_nz]	= CreateCornerShell("px_ny_nz", { unit, -unit, -unit }, { 180, 180, 0 });
	pcImpl->m_cSegments[(int)H3DF::ViewDirection::Mode::px_ny_pz]	= CreateCornerShell("px_ny_pz", { unit, -unit, unit }, { 270, 180, 0 });
}



void H3DF::NavigationCube::CreateCubeWire()
{
#define CreateCylinder(p1, p2) \
	key = key = HC_Insert_Cylinder(&points[p1], &points[p2], 0.005, "none"); \
	ASSERT(key != HC_ERROR_KEY);

	double unit = PRESET::PlaneUnit();

	HPoint points[8];
	// front
	points[0].Set(-unit, -unit, -unit);
	points[1].Set( unit, -unit, -unit);
	points[2].Set( unit,  unit, -unit);
	points[3].Set(-unit,  unit, -unit);
	// back
	points[4].Set(-unit, -unit,  unit);
	points[5].Set( unit, -unit,  unit);
	points[6].Set( unit,  unit,  unit);
	points[7].Set(-unit,  unit,  unit);

	HC_KEY key = HC_ERROR_KEY;
	HC_KEY segKey = HC_Open_Segment("wire");
	ASSERT(segKey != HC_ERROR_KEY);
	{
		Painter::Segment::SetColor("faces", PRESET::LineColor());

		CreateCylinder(0, 1);
		CreateCylinder(1, 2);
		CreateCylinder(2, 3);
		CreateCylinder(3, 0);

		CreateCylinder(4, 5);
		CreateCylinder(5, 6);
		CreateCylinder(6, 7);
		CreateCylinder(7, 4);

		CreateCylinder(0, 4);
		CreateCylinder(1, 5);
		CreateCylinder(2, 6);
		CreateCylinder(3, 7);
	}
	HC_Close_Segment();

#undef CreateCylinder
}

//#define PLANE_EDGE
//#define ROUND_EDGE

HC_KEY H3DF::NavigationCube::CreatePlaneShell(const char* name, const char* text, Triple pos, Triple angle)
{
	double unit = PRESET::PlaneUnit() - PRESET::CornerUnit();

	HPoint points[4];
	points[0].Set(-unit,  unit);
	points[1].Set(-unit, -unit);
	points[2].Set( unit, -unit);
	points[3].Set( unit,  unit);

	int faces[] = {
		4, 0, 1, 2, 3
	};

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		HC_Set_Text_Font("transforms = on");

		HC_KEY shellKey = HC_Insert_Shell(sizeof(points) / sizeof(HPoint), points, sizeof(faces) / sizeof(int), faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_KEY textKey = HC_Insert_Text(0, 0, 0, text);
		ASSERT(textKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY H3DF::NavigationCube::CreateEdgeShell(const char* name, Triple pos, Triple angle)
{
	double width = PRESET::EdgeUnit();
	double height = PRESET::CornerUnit();

#ifdef PLANE_EDGE
	HPoint points[4];
	points[0].Set(-width, -height,      0);
	points[1].Set( width, -height,      0);
	points[2].Set( width,       0, height);
	points[3].Set(-width,       0, height);

	int faces[] = {
		4, 0, 1, 2, 3
	};
#else
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
#endif

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		HC_KEY shellKey = HC_Insert_Shell(sizeof(points) / sizeof(HPoint), points, sizeof(faces) / sizeof(int), faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY H3DF::NavigationCube::CreateCornerShell(const char* name, Triple pos, Triple angle)
{
	double unit = PRESET::CornerUnit();

#ifdef PLANE_EDGE
	HPoint points[3];
	points[0].Set(unit, -unit,    0);
	points[1].Set(unit,     0, unit);
	points[2].Set(   0, -unit, unit);

	int faces[] = {
		3, 0, 1, 2,
	};
#else
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
#endif

	HC_KEY segKey = HC_Open_Segment(name);
	ASSERT(segKey != HC_ERROR_KEY);
	{
		HC_Set_Selectability("faces = on");

		//Segment::SetColor("faces", RGB(0, 0, 255), 0.5);

		HC_KEY shellKey = HC_Insert_Shell(sizeof(points) / sizeof(HPoint), points, sizeof(faces) / sizeof(int), faces);
		ASSERT(shellKey != HC_ERROR_KEY);

		HC_Rotate_Object(angle.x, angle.y, angle.z);
		HC_Translate_Object(pos.x, pos.y, pos.z);
	}
	HC_Close_Segment();

	return segKey;
}



HC_KEY H3DF::NavigationCube::CreateAxis(const char* name, const char* text, HPoint axisEnd, HPoint textCenter, COLORREF rgb)
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



void H3DF::NavigationCube::SetWindowSize(double width, double height, bool openSegment)
{
	NavigationCubeImpl * pcImpl = static_cast<NavigationCubeImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_windowSize.x = width;
	pcImpl->m_windowSize.y = height;

	if (openSegment) {
		OpenCubeSegment();
	}

	double cubeSize = (double)TheCube.Size;
	double fontSize = TheCube.FontSize * TheCube.Size / 100.0;

	double left = 1.0 - 2.0 / width * cubeSize;
	double bottom = 1.0 - 2.0 / height * cubeSize;

	HC_Set_Rendering_Options(PRESET::Format("screen range = (%.6f, 1, %.6f, 1)", left, bottom));
	HC_Set_Text_Font(PRESET::Format("size = %.3f px", fontSize));

	if (openSegment) {
		CloseCubeSegment();
	}
}

#undef PRESET
#undef TheCube
