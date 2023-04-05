#include "StdAfx.h"

#include "3DF.Operator.ObjectSnap.h"
#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"

#include "3DF.Line.h"
#include "3DF.Circle.h"
#include "3DF.Point.h"

#include "3DF.Camera.h"
#include "3DF.Color.h"

#include "3DF.Visibility.h"

#include "../3DF.Signal.Connector.h"

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
}

//== 1. Object Snap 계산 =============================================================================== 

// 1. 주어진 Selection Object를 이용해서 연관된 Object Snap Point를 계산한다.
// 이 함수에서 개별요소의 Object Snap를 구하고, 연관된 요소들의 Object Snap를 구한다. 구하는 Object Snap은 각각의 
// 요소에서 End, Mid, Near, Center등을 구하고 연관된 Entity에서 Intersection, Perpendicular, Tangent등을 구한다.
// 구해진 값은 m_aSnapItems에 저장된다. SnapItem에는 연관된 Key값, Point, Snap Type등이 저장된다.

void Operator::ObjectSnap::CalculationObjectSnapPoint(TDF::SelectionResults & cInItems)
{
	// 단일 Object Snap Point를 계산한다.
	POSITION pcPosition = cInItems.GetHeadPosition();
	while (nullptr != pcPosition) {
		SelectionItem * pcItem = cInItems.GetNext(pcPosition);
		SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

		const WorldPoint cWorldPoint = pcImpl->cWorldPoint;
		const WindowPoint cWindowPoint = pcImpl->cWindowPoint;

		Key * pcKey = nullptr;
		pcItem->ShowSelectedItem(pcKey);

		// Line Key 처리
		if (TDF::Type::LineKey == pcKey->Type()) {
			CalculationLienObjectSnapPoint(pcKey, cWindowPoint);
		}
	}

	// 상호간의 Object Snap Point를 계산한다.
	pcPosition = cInItems.GetHeadPosition();

	while (nullptr != pcPosition) {
		SelectionItem * pcItem = cInItems.GetNext(pcPosition);

		if (nullptr == pcPosition) {
			continue;
		}

		SelectionItem * pcNextItem = cInItems.GetAt(pcPosition);

		Key * pcSelection = nullptr;
		Key * pcNextSelection = nullptr;
		if (true == pcItem->ShowSelectedItem(pcSelection) && pcNextItem->ShowSelectedItem(pcNextSelection)) {
 			if (Type::LineKey == pcSelection->Type() && Type::LineKey == pcNextSelection->Type()) {
 				//CalculationLienAndLineObjectSnapPoint(*((LineKey *)pcSelection), *((LineKey *)pcNextSelection));
 			}
		}
	}
}

//== 2. 단일 Geometry Object Snap 계산 ==============================================================

// 2-1. Line Object Snap 계산 (EndPoint, MidPoint, NearPoint를 계산)
bool Operator::ObjectSnap::CalculationLienObjectSnapPoint(const Key * pcLine, const WindowPoint & cInPoint)
{
	if (Type::LineKey != pcLine->Type()) {
		return false;
	}

	LineKey & cLine = *(LineKey *)pcLine;

	WorldPointArray aPoints;
	cLine.ShowPoints(aPoints);

	size_t nCount = aPoints.GetCount();

	if (1 >= nCount) {
		return false;
	}

	SnapItem * psSnapItem = nullptr;

	// End Point 처리
	Point cSP, cEP;
	if (true == cLine.GetEndPoint(cSP, cEP)) {
		AddSnapItem((Key *)pcLine, cSP, SnapType::EndPoint);
		AddSnapItem((Key *)pcLine, cEP, SnapType::EndPoint);
	}

	// Mid Point 처리
	Point cMP;
	if(true == cLine.GetMidPoint(cMP)) {
		AddSnapItem((Key *)pcLine, cMP, SnapType::MidPoint);
	}

	// Near Point 처리
	WorldPoint cNearPoint;
	if (true == cLine.NearPoint(*m_pcWindow, cInPoint, cNearPoint)) {
		AddSnapItem((Key *)pcLine, cNearPoint, SnapType::NearPoint);
		AddSnapItem((Key *)pcLine, cInPoint, SnapType::NearPoint);
	}

	return true;
}

//== 3. 2개의 Geometry Object Snap 계산 =============================================================

// 3-1. Line & Line 관련 Object Snap을 계산, Intersection
void Operator::ObjectSnap::CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2)
{
	PointArray aIntersectionPoints;
	if (true == cLine1.GetIntersectionPoint(cLine2, aIntersectionPoints)) {
 		AddSnapItem((Key *)&cLine1, aIntersectionPoints[0], SnapType::MidPoint);
 		//AddSnapItem((Key *)&cLine2, aIntersectionPoints[0], SnapType::MidPoint);
	}

/*
	WorldPointArray aPoints;
	cLine1.ShowPoints(aPoints);
	size_t nCount = aPoints.GetCount();
	if (1 >= nCount) {
		return;
	}
	// Intersection 처리
	WorldPoint cIntersectionPoint;
	if (true == cLine1.Intersection(cLine2, cIntersectionPoint)) {
		AddSnapItem((Key *)&cLine1, cIntersectionPoint, SnapType::Intersection);
		AddSnapItem((Key *)&cLine2, cIntersectionPoint, SnapType::Intersection);
	}*/
}

//== Object Snap Draw ==============================================================================

void Operator::ObjectSnap::DrawObjectSnapPoint(TDF::SelectionResults & cInItems)
{
	ResetSnapItem();

	CalculationObjectSnapPoint(cInItems);

	DrawSnapItems();
}

//== Object Snap Point를 그리는 함수 ==================================================================

void Operator::ObjectSnap::DrawSnapItems()
{
	// Camera 정보를 받아옴.
	SegmentKey cScene(m_pcWindow->GetSceneKey());
	CameraKit cCamera;
	cScene.ShowCamera(cCamera);

	SegmentKey cConstruction(m_pcWindow->GetBaseView()->GetConstructionKey());
	cConstruction.ForcedOpen();

	//HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {
		HC_Flush_Contents(".", "geometry, segment");

		Point cPosition;
		cCamera.ShowPosition(cPosition);

		Point cTarget;
		cCamera.ShowTarget(cTarget);

		Vector cViewNormal = cTarget - cPosition;
		cViewNormal.Normalize();

		for (POSITION pcPosition = m_aSnapItems.GetHeadPosition(); nullptr != pcPosition; ) {
			SnapItem * pcItem = m_aSnapItems.GetNext(pcPosition);

			switch (pcItem->eType) 
			{
				case SnapType::EndPoint:
					//cConstruction.InsertCircle(pcItem->cPoint, 2.0, cViewNormal);

					DrawCircle(cConstruction, pcItem->cPoint, cViewNormal, RGB(255, 255, 0), 0.4);
					//DrawEndPoint("EndPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
					break;

				case SnapType::MidPoint:
					DrawMidPoint("MidPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
					break;

				case SnapType::NearPoint:
					DrawNearPoint("NearPoint", pcItem->cPoint, RGB(255, 255, 0), 0.4);
					break;

	// 			case SnapType::Intersection:
	// 				DrawIntersection("Intersection", pcItem->cPoint, RGB(255, 255, 0), 0.4);
	// 				break;
			}
		}
	//} HC_Close_Segment();

	cConstruction.ForcedClose();

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
	cMaterial.SetColor(cDiffuseColor);

	PixelPoint cRadiusPoints[2];
	cRadiusPoints[1].x = 100.0f;

	WorldPoint cWorldPoint1(*m_pcWindow, cRadiusPoints[0]);
	WorldPoint cWorldPoint2(*m_pcWindow, cRadiusPoints[1]);
	float fRadius = cWorldPoint1.DistanceWith(cWorldPoint2);

	SegmentKey cCircle = cConstruction.Subsegment();
	cCircle.SetMaterialMapping("geometry", cMaterial);
	cCircle.GetVisibilityControl().SetFaces(true);
	cCircle.InsertCircle(cPoint, fRadius, cViewNormal);
}

void Operator::ObjectSnap::DrawRectangle(SegmentKey & cConstruction, Point cPoint, Vector cViewNormal, COLORREF nColor, double dWeight)
{
	RGBAColor cDiffuseColor(GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);
	MaterialMappingKit cMaterial;
	cMaterial.SetColor(cDiffuseColor);

	PixelPoint cRadiusPoints[2];
	cRadiusPoints[1].x = 100.0f;

	WorldPoint cWorldPoint1(*m_pcWindow, cRadiusPoints[0]);
	WorldPoint cWorldPoint2(*m_pcWindow, cRadiusPoints[1]);
	float fRadius = cWorldPoint1.DistanceWith(cWorldPoint2);

	SegmentKey cCircle = cConstruction.Subsegment();
	cCircle.SetMaterialMapping("geometry", cMaterial);
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

//== Utility Function ==============================================================================

bool Operator::ObjectSnap::AddSnapItem(Key * pcKey, Point cSnapPoint, SnapType eType)
{
	SnapItem * psSnapItem = new SnapItem();
	psSnapItem->cPoint = cSnapPoint;
	psSnapItem->eType = eType;
	m_aSnapItems.AddTail(psSnapItem);

	return true;
}

void Operator::ObjectSnap::ResetSnapItem()
{
	// m_aSnapItems을 삭제

	POSITION pos = m_aSnapItems.GetHeadPosition();
	while (pos != NULL)
	{
		SnapItem * psSnapItem = m_aSnapItems.GetNext(pos);
		delete psSnapItem;
	}

	m_aSnapItems.RemoveAll();
}