#include "StdAfx.h"

#include "3DF.Operator.ObjectSnap.h"
#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"

#include "3DF.Line.h"
#include "3DF.Circle.h"
#include "3DF.Point.h"

#include "3DF.Math.Matrix.h"

#include "3DF.Camera.h"
#include "3DF.Color.h"
#include "3DF.Material.h"

#include "3DF.Visibility.h"
#include "3DF.VisualEffects.h"

#include "../3DF.Signal.Connector.h"

#include "HDraw.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#define MARKER_OPCODE_START_FILL -1
#define MARKER_OPCODE_RESTART_FILL -2
#define MARKER_OPCODE_END_FILL -3
#define MARKER_OPCODE_ELLIPSE -4
#define MARKER_OPCODE_CIRCULAR_ARC -5
#define MARKER_OPCODE_INFINITE_LINE -6
#define MARKER_OPCODE_INFINITE_RAY -7
#define MARKER_OPCODE_START_CONTRAST_COLOR -8
#define MARKER_OPCODE_START_NORMAL_COLOR -9
#define MARKER_OPCODE_START_RGB_COLOR -10
#define MARKER_OPCODE_START_INDEXED_COLOR -11
#define MARKER_OPCODE_TERMINATE 0
#define MARKER_OPCODE_POINT 1
#define MARKER_OPCODE_LINE_2 2
#define MARKER_OPCODE_LINE_3 3
#define MARKER_OPCODE_LINE_4 4
#define MARKER_OPCODE_LINE_5 5
#define MARKER_OPCODE_LINE_6 6
#define MARKER_OPCODE_LINE_7 7
#define MARKER_OPCODE_LINE_8 8
#define MARKER_OPCODE_LINE_9 9
#define MARKER_OPCODE_LINE_10 10
#define MARKER_OPCODE_LINE_11 11
#define MARKER_OPCODE_LINE_12 12
#define MARKER_OPCODE_LINE_13 13

//#define USE_IMAGE

#ifdef USE_IMAGE

#include "../Resource.h"
#include "../dllmain.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")



namespace OSnap
{
	Bitmap* LoadPngFromResource(UINT id)
	{
		IStream* pStream = nullptr;
		Gdiplus::Bitmap* pBitmap = nullptr;

		HRSRC hResource = ::FindResource(Application::Instance, MAKEINTRESOURCE(id), L"PNG");
		DWORD dwResourceSize = ::SizeofResource(Application::Instance, hResource);
		HGLOBAL hGlobalResource = ::LoadResource(Application::Instance, hResource);
		LPVOID pData = LockResource(hGlobalResource);

		HGLOBAL hGlobal = ::GlobalAlloc(GHND, dwResourceSize);
		LPVOID pBuffer = ::GlobalLock(hGlobal);
		memcpy_s(pBuffer, dwResourceSize, pData, dwResourceSize);
		HRESULT hResult = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
		if (SUCCEEDED(hResult)) {
			// pStream now owns the global handle and will invoke GlobalFree on release
			hGlobal = nullptr;
			pBitmap = new Gdiplus::Bitmap(pStream);
		}

		if (pStream) {
			pStream->Release();
			pStream = nullptr;
		}

		return pBitmap;
	}

	class GdiLoader
	{
	public:

		GdiLoader()
		{
			GdiplusStartup(&token, &input, nullptr);
		}

		~GdiLoader()
		{
			GdiplusShutdown(token);
		}

	private:

		GdiplusStartupInput input;
		ULONG_PTR token;
	};

	GdiLoader Initializer;



	const char* Format = "rgba, size=12 pixels";
	int Width = 32;
	int Height = 32;

	class ImageLoader
	{
	public:

		ImageLoader(UINT id)
		{
			Id = id;
		}

		~ImageLoader()
		{
			REMOVE_ARRAY(Buffer);
		}

		void Load()
		{
			Bitmap* pBitmap = LoadPngFromResource(Id);
			ASSERT(pBitmap != nullptr);
			ASSERT(pBitmap->GetWidth() > 0 && pBitmap->GetHeight() > 0);

			Buffer = new BYTE[Width * Height * 4];
			Color color;
			int index = 0;

			for (int y = 0; y < Height; y++) {
				for (int x = 0; x < Width; x++) {
					pBitmap->GetPixel(x, y, &color);
					Buffer[index++] = color.GetR();
					Buffer[index++] = color.GetG();
					Buffer[index++] = color.GetB();
					Buffer[index++] = color.GetA();
				}
			}

			REMOVE_POINTER(pBitmap);
		}

		UINT Id = 0;
		BYTE* Buffer = nullptr;
	};

	ImageLoader Center(IDF_OSNAP_CENTER);
	ImageLoader End(IDF_OSNAP_END);
	ImageLoader Intersection(IDF_OSNAP_INTERSECTION);
	ImageLoader Mid(IDF_OSNAP_MID);
	ImageLoader Nearest(IDF_OSNAP_NEAREST);
	ImageLoader Node(IDF_OSNAP_NODE);
	ImageLoader Perpendicular(IDF_OSNAP_PERPENDICULAR);
	ImageLoader Quadrant(IDF_OSNAP_QUADRANT);
	ImageLoader Tangent(IDF_OSNAP_TANGENT);
}

#endif

USING_3DF_NAMESPACE


Operator::ObjectSnap::ObjectSnap(WindowKey * pcWindow)
{
	m_pcWindow = pcWindow;

	SegmentKey cConstruction(m_pcWindow->GetBaseView()->GetConstructionKey());

	// Snap Point Segment 설정
	m_cSnapPointSegment = cConstruction.Subsegment(L"SnapPoint");

	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetEdgeColor(RGBAColor(0, 0, 0));
	cMaterialMapping.SetFaceColor(RGBAColor(1, 1, 1));
	m_cSnapPointSegment.SetMaterialMapping(cMaterialMapping);
	m_cSnapPointSegment.GetVisibilityControl().SetFaces(true);
	m_cSnapPointSegment.GetEdgeAttributeControl().SetWeight(4.0, Edge::SizeUnits::Pixels);
//	m_cSnapPointSegment.GetVisualEffectsControl().SetAntiAliasing(false);


	char chDriverOption[MVO_BUFFER_SIZE];
	char chRenderingOption[MVO_BUFFER_SIZE];
	HC_Open_Segment_By_Key(m_cSnapPointSegment.KeyValue());
	HC_Set_Rendering_Options("lines=on");
	HC_Show_Net_Driver_Options(chDriverOption);
	HC_Show_Net_Rendering_Options(chRenderingOption);
	HC_Close_Segment();

}

//== Mouse Event ===================================================================================
int Operator::ObjectSnap::NoButtonDownAndMove(HEventInfo & cInEvent)
{
	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	// Event에서 들어온 Mouse 위치를 이용해서 Snap Point가 선택된 경우 (주어진 Pixel 범위내에 있을 때), 
	// Snap Point에 선택 Flag을 주어서 선택된 효과를 주도록 한다.
	for (auto & pcSnapItem : m_vSnapItems) {

		if (Type::NearPoint == pcSnapItem->eType) {
			continue;
		}

		PixelPoint cMousePoint(cInEvent.GetMousePixelPos());
		PixelPoint cPixelPoint(*m_pcWindow, pcSnapItem->cPoint);

		double dDist = cPixelPoint.DistanceWith(cMousePoint);

		if (15 > dDist) {
			pcSnapItem->eStatus = ObjectSnap::Status::Selected;
			DrawSnapItem(pcSnapItem, cCameraInfo);
			return HLISTENER_PASS_EVENT;
		}
	}

	SelectionOptionsKit cSelectOption;
	//cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetProximity(0.2f).SetSorting(Selection::Sorting::ZSorting);
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetSorting(Selection::Sorting::ZSorting);

	SelectionResults cHighlightSelection;
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cInEvent, cSelectOption, cHighlightSelection);

	// 신규 선택 요소 저장소는 초기화한다.
	m_cNewHighlightSelection.Reset();

	// Selection된 Item들에서 Windows Point의 Z값을 이용해서 Sort하도록 한다.
	// 1.나오는 Item은 이미 Sorting이 되어 있음.
	// 2.맨앞에 나온 요소가 ShellKey이고, 같은 Z값에 LineKey가 있는 경우 LineKey를 사용하도록 한다.
	if (0 < nSelectedCount) {
		SelectionResultsIterator cIter = cHighlightSelection.GetIterator();

		if (true == cIter.IsValid()) {
			SelectionItem * pcItem = cIter.GetItem();

			Key cSelectKey;
			pcItem->ShowSelectedItem(cSelectKey);

			// 나오는 요소의 종류를 확인한다.
			TDF::Type eType = cSelectKey.Type();

			//m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));

			// 첫번째 요소가 ShellKey인 경우 다음 요소와의 거리를 측정해서 공차 범위안에 Line이 있는 경우는 Line을 선택한다.
			// 오차값으로 사용하기에는 값이 너무 크다
/*
			if (Type::ShellKey == eType) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));

				cIter.Next();

				if (true == cIter.IsValid()) {
					SelectionItem * pcNextItem = cIter.GetItem();
					Key cNextSelectKey;
					pcNextItem->ShowSelectedItem(cNextSelectKey);
					// 나오는 요소의 종류를 확인한다.
					Type eType = cNextSelectKey.Type();
					if (Type::LineKey == eType) {
						WorldPoint cItemPoint;
						pcItem->ShowSelectionPosition(cItemPoint);

						WorldPoint cNextItemPoint;
						pcNextItem->ShowSelectionPosition(cNextItemPoint);

						// 두개의 거리를 측정한다.
						double dDistance = cItemPoint.DistanceWith(cNextItemPoint);
						TRACE(L"Dist: %f\n", dDistance);

						m_cNewHighlightSelection.PushBack(new SelectionItem(*pcNextItem));
					}
				}
			}*/
			if (TDF::Type::LineKey == eType) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));
			}
		}
	}

	HighlightOptionsKit cHighlightOptions;
	cHighlightOptions.SetNotification(false);

	bool bForceUpdate = false;

	// Old와 New가 다르면 Old를 Unhiglight하고 Reset 시킨다.
	if (0 < m_cOldHighlightSelection.GetCount() && m_cOldHighlightSelection != m_cNewHighlightSelection) {
		m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection, cHighlightOptions);
		m_cOldHighlightSelection.Reset();
		ClearSnapItems(false);
		bForceUpdate = true;
	}

	// 	새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	// 	m_cNewHighlightSelection.LeaveType((DWORD)TDF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(m_cNewHighlightSelection)) {
		if (5 < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(5);
		}
	}

	m_cOldHighlightSelection = m_cNewHighlightSelection;

	if (0 < m_cNewHighlightSelection.GetCount()) {
		m_pcWindow->GetHighlightControl().Highlight(m_cNewHighlightSelection, cHighlightOptions);
		bForceUpdate = true;

		// 사전 선택된 Object Snap Point 삭제
		ResetSnapItem();

		// Object Snap Point를 계산한다.
		CalculationObjectSnapPoint(m_cHighlightSelection);

		// Snap Item을 그린다.
		DrawSnapItems(false);
	}

	/*
		else {
			// Object Snape 등을 지우도록 한다.
			HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {
				HC_Flush_Contents(".", "geometry, segment");
			} HC_Close_Segment();

			if (false == bForceUpdate) {
				// m_pcWindow->GetBaseView()->Update();
			}
		}*/

	if (true == bForceUpdate) {
		m_pcWindow->GetBaseView()->ForceUpdate();
	}

	return HLISTENER_PASS_EVENT;

	// PMI Test	Code
/*
	SegmentKey cSecne(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);

	Matrix cMatrix;
	cCamera.ShowMatrix(cMatrix);

	WorldPoint cPoint[2];

	cPoint[0] = m_cClickPoint;
	cPoint[1] = cInEvent.GetMouseWorldPos();

	Vector cXAixs = cMatrix.XAxis();
	Vector cYAixs = cMatrix.YAxis();
	Vector cOrigin = cMatrix.Origin();

	Point2D cP1 = cPoint[0].DropPoint(cOrigin, cXAixs, cYAixs);
	Point2D cP2 = cPoint[1].DropPoint(cOrigin, cXAixs, cYAixs);

	// Test Object Snap
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey()); {
		HDraw::Test(GetView(), cMatrix, cP1, cP2);
	} HC_Close_Segment();

	GetView()->Update();
*/

	return HLISTENER_PASS_EVENT;
}


//== 1. Object Snap 계산 =============================================================================== 

// 1. 주어진 Selection Object를 이용해서 연관된 Object Snap Point를 계산한다.
// 이 함수에서 개별요소의 Object Snap를 구하고, 연관된 요소들의 Object Snap를 구한다. 구하는 Object Snap은 각각의 
// 요소에서 End, Mid, Near, Center등을 구하고 연관된 Entity에서 Intersection, Perpendicular, Tangent등을 구한다.
// 구해진 값은 m_aSnapItems에 저장된다. SnapItem에는 연관된 Key값, Point, Snap Type등이 저장된다.
void Operator::ObjectSnap::CalculationObjectSnapPoint(TDF::SelectionResults & cInItems)
{
	// 단일 Object Snap Point를 계산한다. 이 경우 첫번째 Item만 처리한다.
	SelectionItem * pcItem = cInItems.Front();

	if (nullptr != pcItem) {
		WorldPoint cWorldPoint;
		WindowPoint cWindowPoint;

		pcItem->ShowSelectionPosition(cWorldPoint);
		pcItem->ShowSelectionPosition(cWindowPoint);

		Key cKey;
		pcItem->ShowSelectedItem(cKey);
		TDF::Type eType = cKey.Type();

		KeyPath cPath;
		pcItem->ShowPath(cPath);

		Matrix cMatrix;
		cPath.ShowNetModellingMatrix(cMatrix);
	
		// Line Key 처리
		if (TDF::Type::LineKey == eType) {
			CalculationLienObjectSnapPoint(cKey, cWindowPoint, cMatrix);
		}
	}

	//----- 상호간의 Object Snap Point를 계산한다. -----

	SelectionResultsIterator cIter = cInItems.GetIterator();
	
	// 제일 첫번째 Item을 메인으로 해서 계산을 진행한다.

	if(false == cIter.IsValid()) { // 정상적인 상태인지 확인.
		return;
	}

	pcItem = cIter.GetItem();

	KeyPath cPath;
	pcItem->ShowPath(cPath);

	Matrix cMatrix;
	cPath.ShowNetModellingMatrix(cMatrix);

	Key cSelection;
	if (false == pcItem->ShowSelectedItem(cSelection)) {
		return;
	}

	// 다음 Item을 가져온다.
	cIter.Next();

	while (cIter.IsValid()) {
		SelectionItem * pcNextItem = cIter.GetItem();
		KeyPath cNextPath;
		pcNextItem->ShowPath(cNextPath);
		Matrix cNextMatrix;
		cNextPath.ShowNetModellingMatrix(cNextMatrix);
		Key cNextSelection;
		if (true == pcNextItem->ShowSelectedItem(cNextSelection)) {
			if (TDF::Type::LineKey == cSelection.Type() && TDF::Type::LineKey == cNextSelection.Type()) {
				LineKey cLine = LineKey(cSelection);
				LineKey cNextLine = LineKey(cNextSelection);
 				CalculationLienAndLineObjectSnapPoint(cLine, cNextLine, cMatrix, cNextMatrix);
 			}
		}
		// 다음 Item을 가져온다.
		cIter.Next();
	}
}

//== 2. 단일 Geometry Object Snap 계산 ==============================================================

// 2-1. Line Object Snap 계산 (EndPoint, MidPoint, NearPoint를 계산)
bool Operator::ObjectSnap::CalculationLienObjectSnapPoint(const Key & cInLineKey, const WindowPoint & cInPoint, const MatrixKit & cModelingMatrix)
{
	if (TDF::Type::LineKey != cInLineKey.Type()) {
		return false;
	}

	LineKey cLine = LineKey(cInLineKey);

	WorldPointArray aPoints;
	cLine.ShowPoints(aPoints);

	size_t nCount = aPoints.size();

	if (1 >= nCount) {
		return false;
	}

	CircleKit cCircle;
	if (true == TDF::Math::GetCircle(aPoints, cCircle)) {
		Point cCenter;
		cCircle.ShowCenter(cCenter);
		cCenter = cModelingMatrix.Transform(cCenter);
		AddSnapItem(cLine, cCenter, Type::MidPoint);
		return true;
	}

	// End Point 처리
	Point cSP, cEP;
	if (true == cLine.GetEndPoint(cSP, cEP)) {
		cSP = cModelingMatrix.Transform(cSP);
		cEP = cModelingMatrix.Transform(cEP);
		AddSnapItem(cLine, cSP, Type::EndPoint);
		AddSnapItem(cLine, cEP, Type::EndPoint);
	}

	// Mid Point 처리
	Point cMP;
	if(true == cLine.GetMidPoint(cMP)) {
		cMP = cModelingMatrix.Transform(cMP);
		AddSnapItem(cLine, cMP, Type::MidPoint);
	}

	// Near Point 처리
	WorldPoint cNearPoint;
	if (true == cLine.NearPoint(*m_pcWindow, cModelingMatrix, cInPoint, cNearPoint)) {
		AddSnapItem(cLine, cNearPoint, Type::NearPoint);
	}

	return true;
}

//== 3. 2개의 Geometry Object Snap 계산 =============================================================

// 3-1. Line & Line 관련 Object Snap을 계산, Intersection
void Operator::ObjectSnap::CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2)
{
	// 교차점 처리
	PointArray aIntersectionPoints;
	if (true == cLine1.GetIntersectionPoint(cLine2, cMatrix1, cMatrix2, aIntersectionPoints)) {
		// 찾아온 교차점을 SnapItem에 추가한다.
		for (size_t nIndex = 0; nIndex < aIntersectionPoints.size(); nIndex++) {
			AddSnapItem(cLine1, aIntersectionPoints[nIndex], Type::MidPoint);
		}
	}
}

//== Object Snap Point를 그리는 함수 ==================================================================

void Operator::ObjectSnap::DrawSnapItems(bool bUpdate)
{
	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	m_cSnapPointSegment.Open(); {

		HC_Flush_Contents(".", "geometry, segment");

		m_cSnapPointSegment.SetModellingMatrix(cCameraInfo.cMatrix);

		for (auto pcItem : m_vSnapItems) {
			Point2D cDropPoint = pcItem->cPoint.DropPoint(cCameraInfo.cOrigin, cCameraInfo.cXAixs, cCameraInfo.cYAixs);

			bool bSelected = false;
			if (Status::Selected == pcItem->eStatus) {
				bSelected = true;
			}

			CString strSnapType;
			HDraw::DrawSnapPoint(cCameraInfo.dObjectSnapRadius, cDropPoint, bSelected);
		}

	} m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

void Operator::ObjectSnap::DrawSnapItems(CamerInformation & cInCameraInfo, bool bUpdate)
{
	m_cSnapPointSegment.Open(); {

		HC_Flush_Contents(".", "geometry, segment");

		m_cSnapPointSegment.SetModellingMatrix(cInCameraInfo.cMatrix);

		for (auto pcItem : m_vSnapItems) {
			Point2D cDropPoint = pcItem->cPoint.DropPoint(cInCameraInfo.cOrigin, cInCameraInfo.cXAixs, cInCameraInfo.cYAixs);

			bool bSelected = false;
			if (Status::Selected == pcItem->eStatus) {
				bSelected = true;
			}

			CString strSnapType;
			HDraw::DrawSnapPoint(cInCameraInfo.dObjectSnapRadius, cDropPoint, bSelected);
		}

	} m_cSnapPointSegment.Close();

	if(true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

void Operator::ObjectSnap::DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate)
{
	m_cSnapPointSegment.Open(); {

		HC_Flush_Contents(".", "geometry, segment");

		m_cSnapPointSegment.SetModellingMatrix(cInCameraInfo.cMatrix);

		Point2D cDropPoint = pcInItem->cPoint.DropPoint(cInCameraInfo.cOrigin, cInCameraInfo.cXAixs, cInCameraInfo.cYAixs);

		bool bSelected = false;
		if (Status::Selected == pcInItem->eStatus) {
			bSelected = true;
		}

		CString strSnapType;
		HDraw::DrawSnapPoint(cInCameraInfo.dObjectSnapRadius, cDropPoint, bSelected);

	} m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

bool Operator::ObjectSnap::ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo)
{
	if(nullptr == m_pcWindow) {
		return false;
	}

	SegmentKey cSecne(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);
	cCamera.ShowMatrix(cOutInfo.cMatrix);

	cOutInfo.cXAixs = cOutInfo.cMatrix.XAxis();
	cOutInfo.cYAixs = cOutInfo.cMatrix.YAxis();
	cOutInfo.cOrigin = cOutInfo.cMatrix.Origin();

	PixelPoint cPixelPoint1;
	PixelPoint cPixelPoint2(fInRadius, 0, 0);
	WorldPoint cWorldPoint1(*m_pcWindow, cPixelPoint1);
	WorldPoint cWorldPoint2(*m_pcWindow, cPixelPoint2);
	Vector cVector = cWorldPoint2 - cWorldPoint1;
	cOutInfo.dObjectSnapRadius = cVector.Length();

	return true;
}

void Operator::ObjectSnap::DrawSnapItems1()
{
	HC_KEY nConstructionKey = m_pcWindow->GetBaseView()->GetConstructionKey();

	// Camera 정보를 받아옴.
	SegmentKey cScene(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cScene.ShowCamera(cCamera);

	SegmentKey cConstruction(m_pcWindow->GetBaseView()->GetConstructionKey());
	cConstruction.Open();

	HC_Flush_Contents(".", "geometry, segment");

	Point cPosition;
	cCamera.ShowPosition(cPosition);

	Point cTarget;
	cCamera.ShowTarget(cTarget);

	Vector cViewNormal = cTarget - cPosition;
	cViewNormal.Normalize();

	Vector cYAxis;
	cCamera.ShowUpVector(cYAxis);
	cYAxis.Normalize();

	Vector cXAxis = cYAxis.Cross(cViewNormal);

	for (auto pcItem : m_vSnapItems) {
		switch (pcItem->eType)
		{
			case Type::EndPoint:
				//cConstruction.InsertCircle(pcItem->cPoint, 2.0, cViewNormal);

				//DrawCircle(cConstruction, pcItem->cPoint, cViewNormal, RGB(255, 255, 0), 0.4);
				DrawEndPoint("EndPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
				break;

			case Type::MidPoint:
				DrawMidPoint("MidPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
				break;

			case Type::NearPoint:
				DrawNearPoint("NearPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
				break;

// 			case SnapType::Intersection:
// 				DrawIntersection("Intersection", pcItem->cPoint, RGB(255, 255, 0), 0.4);
// 				break;
		}
	}

	cConstruction.Close();

	m_pcWindow->GetBaseView()->Update();
}

void Operator::ObjectSnap::DrawCenterMark(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
#ifdef USE_IMAGE
	HC_Open_Segment(pchSegmentName);
	{
		HC_Insert_Image(cPoint.x, cPoint.y, cPoint.z, OSnap::Format, OSnap::Width, OSnap::Height, OSnap::Center.Buffer);
	}
	HC_Close_Segment();
#else
	HC_Open_Segment(pchSegmentName);
	{
/*
		if (true == bTempDrawFlag) {
			HC_Set_Rendering_Options("no simple reflection");
			HC_Set_Rendering_Options("no simple shadow, no frame buffer effects");
			HC_Set_Rendering_Options("depth range = (0.1,0.2)");
			HC_Set_Visibility("shadows = off");
			HC_Set_Visibility("cutting planes = off");
			HC_Set_User_Options("on top");
		}
*/

		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapCenterMark");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
#endif
}

void Operator::ObjectSnap::DrawBox(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapBox");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::DrawEndPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
#ifdef USE_IMAGE
	HC_Open_Segment(pchSegmentName);
	{
		HC_Insert_Image(cPoint.x, cPoint.y, cPoint.z, OSnap::Format, OSnap::Width, OSnap::Height, OSnap::Center.Buffer);
	}
	HC_Close_Segment();
#else
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapEndPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
#endif
}

void Operator::ObjectSnap::DrawMidPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
#ifdef USE_IMAGE
	HC_Open_Segment(pchSegmentName);
	{
		HC_Insert_Image(cPoint.x, cPoint.y, cPoint.z, OSnap::Format, OSnap::Width, OSnap::Height, OSnap::Center.Buffer);
	}
	HC_Close_Segment();
#else
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapMidPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
#endif
}

void Operator::ObjectSnap::DrawNearPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
#ifdef USE_IMAGE
	HC_Open_Segment(pchSegmentName);
	{
		HC_Insert_Image(cPoint.x, cPoint.y, cPoint.z, OSnap::Format, OSnap::Width, OSnap::Height, OSnap::Center.Buffer);
	}
	HC_Close_Segment();
#else
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Variable_Marker_Size("3 oru");

		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapNearPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
#endif
}

void Operator::ObjectSnap::DrawCircle(SegmentKey & cConstruction, Point cPoint, Vector cViewNormal, COLORREF nColor, double dWeight)
{
	RGBAColor cDiffuseColor(GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);
	MaterialMappingKit cMaterial;
	cMaterial.SetLineColor(cDiffuseColor);

	PixelPoint cRadiusPoints[2];
	cRadiusPoints[1].x = 100.0f;

	WorldPoint cWorldPoint1(*m_pcWindow, cRadiusPoints[0]);
	WorldPoint cWorldPoint2(*m_pcWindow, cRadiusPoints[1]);

	float fRadius = cWorldPoint1.DistanceWith(cWorldPoint2);

	SegmentKey cCircle = cConstruction.Subsegment();
 	cCircle.SetMaterialMapping(cMaterial);
 	cCircle.GetVisibilityControl().SetFaces(true);
 	cCircle.InsertCircle(cPoint, fRadius, cViewNormal);


	//cCircle.ForcedOpen();
	//static HC_KEY InsertRectangle(const char * seg, float x0, float y0, float x1, float y1, float z = 0.0f, bool fill = false)
	
	//cCircle.ForcedClose();
}

void Operator::ObjectSnap::DrawRectangle(SegmentKey & cConstruction, Point cPoint, Vector cViewNormal, COLORREF nColor, double dWeight)
{
	RGBAColor cDiffuseColor(GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);
	MaterialMappingKit cMaterial;
	cMaterial.SetLineColor(cDiffuseColor);

	PixelPoint cRadiusPoints[2];
	cRadiusPoints[1].x = 100.0f;

	WorldPoint cWorldPoint1(*m_pcWindow, cRadiusPoints[0]);
	WorldPoint cWorldPoint2(*m_pcWindow, cRadiusPoints[1]);
	float fRadius = cWorldPoint1.DistanceWith(cWorldPoint2);

	SegmentKey cCircle = cConstruction.Subsegment();
	cCircle.SetMaterialMapping(cMaterial);
	cCircle.GetVisibilityControl().SetFaces(true);
	cCircle.InsertCircle(cPoint, fRadius, cViewNormal);
}

void Operator::ObjectSnap::CreateGlyph()
{
	HC_Open_Segment("/"); {
		const char chEndPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			127, -127,
			-127, -127,
			-127, 127,

			MARKER_OPCODE_LINE_5,
			-107, 107,
			107, 107,
			107, -107,
			-107, -107,
			-107, 107,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapEndPoint", sizeof(chEndPointData), chEndPointData);

		const char chMidPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_4,
			0, 127,
			-127, -93,
			127, -93,
			0, 127,

			MARKER_OPCODE_LINE_4,
			0, 87,
			-92, -73,
			92, -73,
			0, 87,

			MARKER_OPCODE_LINE_4,
			0, 97,
			-101, -77,
			101, -77,
			0, 97,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapMidPoint", sizeof(chMidPointData), chMidPointData);

		const char chNearPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			-127, -127,
			127, -127,
			-127, 127,

// 			MARKER_OPCODE_LINE_5,
// 			-127, 127,
// 			127, 127,
// 			-127, -127,
// 			127, -127,
// 			-127, 127,

// 			-127, 127,
// 			127, 127,
// 			127, -127,
// 			-127, -127,
// 			-127, 127,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapNearPoint", sizeof(chNearPointData), chNearPointData);

		const char chBoxData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			127, -127,
			-127, -127,
			-127, 127,
			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapBox", sizeof(chBoxData), chBoxData);

		const char chCenterMarkData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			// 우측 상단 원호
			MARKER_OPCODE_CIRCULAR_ARC,
			127, 0,
			90, 90,
			0, 127,

			// 좌측 상단 원호 채움
			MARKER_OPCODE_START_FILL,
			MARKER_OPCODE_CIRCULAR_ARC,
			0, 127,
			-90, 90,
			-127, 0,
			MARKER_OPCODE_LINE_2,
			-127, 0,
			0, 0,
			MARKER_OPCODE_LINE_2,
			0, 0,
			0,127,
			MARKER_OPCODE_END_FILL,

			// 좌측 하단 원호
			MARKER_OPCODE_CIRCULAR_ARC,
			-127, 0,
			-90, -90,
			0, -127,

			// 우측 하단 원호 채움
			MARKER_OPCODE_START_FILL,
			MARKER_OPCODE_CIRCULAR_ARC,
			0, -127,
			90, -90,
			127,0,
			MARKER_OPCODE_LINE_2,
			127, 0,
			0, 0,
			MARKER_OPCODE_LINE_2,
			0, 0,
			0, -127,
			MARKER_OPCODE_END_FILL,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapCenterMark", sizeof(chCenterMarkData), chCenterMarkData);

	} HC_Close_Segment();
}

void Operator::ObjectSnap::LoadResource()
{
// 	OSnap::Center.Load();
// 	OSnap::End.Load();
// 	OSnap::Intersection.Load();
// 	OSnap::Mid.Load();
// 	OSnap::Nearest.Load();
// 	OSnap::Node.Load();
// 	OSnap::Perpendicular.Load();
// 	OSnap::Quadrant.Load();
// 	OSnap::Tangent.Load();
}

void Operator::ObjectSnap::ClearSnapItems(bool bUpdate)
{
	m_cSnapPointSegment.Open(); {
		HC_Flush_Contents(".", "geometry, segment");
	} m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

//== Utility Function ==============================================================================

bool Operator::ObjectSnap::AddSnapItem(Key & cInKey, Point cSnapPoint, Type eType)
{
	SnapItem * psSnapItem = new SnapItem();
	psSnapItem->cPoint = cSnapPoint;
	psSnapItem->eType = eType;
	m_vSnapItems.push_back(psSnapItem);

	return true;
}

void Operator::ObjectSnap::ResetSnapItem()
{
	// m_aSnapItems을 삭제
	for (auto pcSnapItem : m_vSnapItems) {
		delete pcSnapItem;
	}

	m_vSnapItems.clear();
}