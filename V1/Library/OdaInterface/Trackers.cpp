#include "stdafx.h"

#include "Renderer.h"
#include "Trackers.h"
#include "UserIO.h"

#include "DbDatabase.h"
#include "DbTextStyleTable.h"
#include "DbTextStyleTableRecord.h"
#include "Ge/GeCircArc2d.h"
#include "Ge/GeLine2d.h"
#include "Gi/GiCommonDraw.h"
#include "Gi/GiTextStyle.h"

//**************************************************************************************************

OdGePoint2d TrackerBase::PixelDensity = OdGePoint2d::kOrigin;



namespace
{
    const OdString StyleName = L"__#__RubberBand";
    const OdString FontName = L"Segoe UI Light";

    const int TextHeight = 13;
    const int BandColorIndex = 40;
    // Ctl::EColor::DarkGray
    const OdCmEntityColor LineColor(0xA9, 0xA9, 0xA9);
    // Ctl::EColor::MidiumBack
    const OdCmEntityColor BackColor(0x48, 0x48, 0x48);

    OdCmColor TextColor()
    {
        OdCmColor color;
        color.setColorMethod(OdCmEntityColor::kByColor);
        // Ctl::EColor::Silver
        color.setRGB(0xC0, 0xC0, 0xC0);

        return color;
    }
}

//**************************************************************************************************

void TrackerParams::Set(UserIO* pIo)
{
    DEBUG_VALID(pIo);

    UserIoPtr = pIo;
    RendererPtr = UserIoPtr->State.RendererPtr;
    DatabasePtr = RendererPtr->DatabasePtr;
}

//**************************************************************************************************

void TrackerBase::SetPixelDensity(OdGsView* pView)
{
    pView->getNumPixelsInUnitSquare(OdGePoint3d::kOrigin, PixelDensity, false);
}



double TrackerBase::GetPixelSize(int pixel)
{
    return (double)pixel / PixelDensity.x;
}

//--------------------------------------------------------------------------------------------------

void TrackerBase::Initialize(OdGsView* pView)
{
    Valid = true;
}



void TrackerBase::Terminate(OdGsView* pView)
{
    Valid = false;
}



OdUInt32 GraphTracker::subSetAttributes(OdGiDrawableTraits* pDt) const
{
    return kDrawableIsAnEntity;
}

//**************************************************************************************************

bool GraphTracker::subWorldDraw(OdGiWorldDraw* pWd) const
{
    //OutputDebugString(L"GraphTracker::subWorldDraw()\n");
    return false;
}



void GraphTracker::Invalidate()
{
    //OutputDebugString(L"GraphTracker::Invalidate()\n");
    if (GsModelPtr != nullptr) {
        OdGiDrawable* pParent = nullptr;
        GsModelPtr->onModified(this, pParent);
    }
}



void GraphTracker::Initialize(OdGsView* pView)
{
    GsViewPtr = pView;

	if (pView != nullptr && pView->device() != nullptr && GsModelPtr == nullptr) {
	    GsModelPtr = pView->device()->createModel().detach();
		if (GsModelPtr != nullptr) {
			// Skip Z-buffer for 2d drawables.
			//GsModelPtr->setRenderType(OdGsModel::kDirect);

            //:CHECK - Use main Z-buffer
            GsModelPtr->setRenderType(OdGsModel::kMain);
		}
	}

    pView->add(this, GsModelPtr);

    __super::Initialize(pView);
}



void GraphTracker::Terminate(OdGsView* pView)
{
    pView->erase(this);

    __super::Terminate(pView);
}

//**************************************************************************************************

void RubberBandTracker::subViewportDraw(OdGiViewportDraw* pVd) const
{
    //OutputDebugString(L"RubberBandTracker::subViewportDraw()\n");
    if (Valid == false) {
        return;
    }

    OdGiDrawFlagsHelper flag1(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
    OdGiDrawFlagsHelper flag2(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawSolidFill);

    if (BandLine) {
        pVd->subEntityTraits().setColor(BandColorIndex);

        OdGeVector3d dir = EndPoint - StartPoint;
        double length = dir.normalizeGetLength();

        OdGePoint3d segments[2];
        segments[0] = StartPoint;
        segments[1] = EndPoint;

        if (length > 0.0) {
            double dashLength = GetPixelSize(20);
            double gapLength = GetPixelSize(10);

            OdGiViewportGeometry& geom = pVd->geometry();
            double drawn = 0.0;

            while (drawn < length) {
                segments[1] = segments[0] + dir * dashLength;
                drawn += dashLength;

                if (drawn > length) {
                    segments[1] = EndPoint;
                    geom.polyline(2, segments);
                    break;
                }

                geom.polyline(2, segments);
                segments[0] = segments[1] + dir * gapLength;

                drawn += gapLength;
            }
        }
        else {
            pVd->geometry().polyline(2, segments);
        }
    }

    const double pointRadius = GetPixelSize(2);

    if (LengthGuide) {
        pVd->subEntityTraits().setTrueColor(LineColor);
        pVd->geometry().polyline(5, Result.LinePoints);

        pVd->subEntityTraits().setFillType(kOdGiFillAlways);
        pVd->geometry().circle(Result.LinePoints[1], pointRadius, OdGeVector3d::kZAxis);
        pVd->geometry().circle(Result.LinePoints[3], pointRadius, OdGeVector3d::kZAxis);

        //:WARNING - release const
        ((RubberBandTracker*)this)->DrawLengthText(pVd);
    }

    if (AngleGuide) {
        int endPoint = EndPoint.y >= StartPoint.y ? 1 : 3;
        OdGePoint3d points[2];
        points[0] = Result.ArcPoints[0];
        points[1] = Result.ArcPoints[endPoint];

        pVd->subEntityTraits().setTrueColor(LineColor);
        pVd->geometry().polyline(2, points);

        try {
            OdGeLine2d geLine1(Result.ArcPoints[1].convert2d(), Result.ArcPoints[2].convert2d());
            OdGeLine2d geLine2(Result.ArcPoints[2].convert2d(), Result.ArcPoints[3].convert2d());
            if (geLine1.isParallelTo(geLine2)) {
                pVd->geometry().polyline(3, Result.ArcPoints + 1);
            }
            else {
                pVd->geometry().circularArc(Result.ArcPoints[1], Result.ArcPoints[2], Result.ArcPoints[3]);
            }
        }
        catch (...) {
        }

        pVd->subEntityTraits().setFillType(kOdGiFillAlways);
        pVd->geometry().circle(Result.ArcPoints[endPoint], pointRadius, OdGeVector3d::kZAxis);

        //:WARNING - release const
        ((RubberBandTracker*)this)->DrawAngleText(pVd);
    }
}

//--------------------------------------------------------------------------------------------------

void RubberBandTracker::Initialize(OdGsView* pView)
{
    __super::Initialize(pView);

    if (HasDynamicInput()) {
        return;
    }

    OdDbTextStyleTablePtr pTextStyleTable = Params.DatabasePtr->getTextStyleTableId().safeOpenObject(OdDb::kForWrite);
    OdDbObjectId styleId = pTextStyleTable->getAt(StyleName);
    if (styleId.isValid() == false) {
        OdDbTextStyleTableRecordPtr pStyle = OdDbTextStyleTableRecord::createObject();
        pStyle->setName(StyleName);
        pStyle->setIsShapeFile(false);
        pStyle->setFont(FontName, false, false, 0, 2);
        //:CHECK - why not?
        pStyle->setXScale(0.8);

        styleId = pTextStyleTable->add(pStyle);
    }

    LengthText = OdDbText::createObject();
    pView->add(LengthText, 0);

    LengthText->setDatabaseDefaults(Params.DatabasePtr);
    LengthText->setNormal(OdGeVector3d::kZAxis);
    LengthText->setColor(TextColor());
    LengthText->setTextStyle(styleId);

    AngleText = OdDbText::createObject();
    pView->add(AngleText, 0);

    AngleText->setDatabaseDefaults(Params.DatabasePtr);
    AngleText->setNormal(OdGeVector3d::kZAxis);
    AngleText->setColor(TextColor());
    AngleText->setTextStyle(styleId);
}



void RubberBandTracker::Terminate(OdGsView* pView)
{
    if (HasDynamicInput() == false) {
        DEBUG_VALID(LengthText);
        DEBUG_VALID(AngleText);

        pView->erase(LengthText);
        pView->erase(AngleText);
    }

    __super::Terminate(pView);
}



void RubberBandTracker::SetBasePoint(const OdGePoint3d& value)
{
    StartPoint = EndPoint = value;
}



void RubberBandTracker::SetValue(const OdGePoint3d& value)
{
    EndPoint = value;

    if (LengthGuide) {
        const double guideOffset = GetPixelSize(80);

        OdGeVector3d dir = EndPoint - StartPoint;
        Result.Length = dir.normalizeGetLength();
        dir.rotateBy(Uio::ToRadian(EndPoint.y >= StartPoint.y ? 90 : 270), OdGeVector3d::kZAxis);
        dir *= guideOffset;

        Result.LinePoints[0] = StartPoint;
        Result.LinePoints[1] = StartPoint + dir;
        Result.LinePoints[3] = EndPoint + dir;
        Result.LinePoints[4] = EndPoint;
        // Length coordinate point
        Result.LinePoints[2] = (Result.LinePoints[1] + Result.LinePoints[3].asVector()) * 0.5;
    }

    if (AngleGuide) {
        OdGeVector3d dir = EndPoint - StartPoint;
        Result.Length = dir.normalizeGetLength();
        Result.Angle = OdGeVector3d::kXAxis.angleTo(dir);
        Result.ArcPoints[0] = StartPoint;

        if (EndPoint.y >= StartPoint.y) {
            Result.ArcPoints[1] = StartPoint + OdGeVector3d::kXAxis * Result.Length;
            Result.ArcPoints[2] = Result.ArcPoints[1];
            Result.ArcPoints[3] = EndPoint;
        }
        else {
            Result.ArcPoints[1] = EndPoint;
            Result.ArcPoints[2] = EndPoint;
            Result.ArcPoints[3] = StartPoint + OdGeVector3d::kXAxis * Result.Length;
        }
        // Angle coordinate point
        Result.ArcPoints[2].rotateBy(Result.Angle * 0.5, OdGeVector3d::kZAxis, StartPoint);
    }
}

//--------------------------------------------------------------------------------------------------

void RubberBandTracker::DrawLengthText(OdGiViewportDraw* pVd)
{
    if (HasDynamicInput() || LengthText.isNull()) {
        return;
    }

    double height = GetPixelSize(TextHeight);
    OdString text = WStr::Format(L"%.3f", Result.Length);

    LengthText->setHeight(height);
    LengthText->setTextString(text);

    OdGeExtents3d extents;
    LengthText->getGeomExtents(extents);
    double width = extents.maxPoint().x - extents.minPoint().x;
    double padding = extents.maxPoint().y - extents.minPoint().y;

    OdGePoint3d pivot = Result.LinePoints[2];
    pivot.x -= width / 2;
    pivot.y -= height / 2;
    //:WARNING - For z-buffer
    pivot.z = 2.0;
    LengthText->setPosition(pivot);

    OdGePoint3d rect[4];
    rect[0].set(pivot.x - padding, pivot.y - padding, 1.0);
    rect[1].set(pivot.x + width + padding, pivot.y - padding, 1.0);
    rect[2].set(pivot.x + width + padding, pivot.y + height + padding, 1.0);
    rect[3].set(pivot.x - padding, pivot.y + height + padding, 1.0);

    pVd->subEntityTraits().setTrueColor(BackColor);
    pVd->geometry().polygon(4, rect);
}



void RubberBandTracker::DrawAngleText(OdGiViewportDraw* pVd)
{
    if (HasDynamicInput() || AngleText.isNull()) {
        return;
    }

    double height = GetPixelSize(TextHeight);
    AngleText->setHeight(height);

    int angle = Uio::ToDegree(Result.Angle);
    AngleText->setTextString(OdString(WStr::Format(L"%d", angle)));

    OdGeExtents3d extents;
    AngleText->getGeomExtents(extents);
    double width = extents.maxPoint().x - extents.minPoint().x;
    double padding = extents.maxPoint().y - extents.minPoint().y;

    OdGePoint3d pivot = Result.ArcPoints[2];
    pivot.x -= width / 2;
    pivot.y -= height / 2;
    pivot.x -= width / 2;
    pivot.y -= height / 2;
    //:WARNING - For z-buffer
    pivot.z = 2.0;
    AngleText->setPosition(pivot);

    OdGePoint3d rect[4];
    rect[0].set(pivot.x - padding, pivot.y - padding, 1.0);
    rect[1].set(pivot.x + width + padding, pivot.y - padding, 1.0);
    rect[2].set(pivot.x + width + padding, pivot.y + height + padding, 1.0);
    rect[3].set(pivot.x - padding, pivot.y + height + padding, 1.0);

    pVd->subEntityTraits().setTrueColor(BackColor);
    pVd->geometry().polygon(4, rect);
}

//**************************************************************************************************

void RubberRectTracker::subViewportDraw(OdGiViewportDraw* pVd) const
{
    OdGePoint3d points[5];
    points[0] = StartPoint;
    points[1] = { EndPoint.x, StartPoint.y, 0 };
    points[2] = EndPoint;
    points[3] = { StartPoint.x, EndPoint.y, 0 };
    points[4] = StartPoint;

    pVd->geometry().polyline(5, points);
}



RubberRectTracker::RubberRectTracker()
{
    BandLine = false;
    LengthGuide = false;
    AngleGuide = false;
}

//**************************************************************************************************

#define SetItem(x,y) \
for (auto item : Buffer) { \
    item->##x(y); \
}

void TrackerStack::Initialize(OdGsView* pView)
{
    SetItem(Initialize, pView);
}

void TrackerStack::Terminate(OdGsView* pView)
{
    SetItem(Terminate, pView);
    Buffer.clear();
}

void TrackerStack::Invalidate(Renderer* pRenderer)
{
    SetItem(Invalidate);
    if (pRenderer != nullptr) {
        pRenderer->SendPaintSignal(pRenderer->UserIo.UseThread);
    }
}

//--------------------------------------------------------------------------------------------------

void TrackerStack::SetValue(double value)
{
    SetItem(SetValue, value);
}

void TrackerStack::SetValue(int value)
{
    SetItem(SetValue, value);
}

void TrackerStack::SetValue(const OdGePoint3d& value)
{
    SetItem(SetValue, value);
}

void TrackerStack::SetValue(CString value)
{
    SetItem(SetValue, value);
}

#undef SetItem

//--------------------------------------------------------------------------------------------------

void TrackerStack::Push(TrackerBase* pTracker)
{
    if (pTracker != nullptr) {
        Buffer.push_back(pTracker);
    }
}



void TrackerStack::Pop(TrackerBase* pTracker)
{
    if (pTracker != nullptr) {
        if (pTracker == Buffer.back()) {
            Buffer.pop_back();
            DEBUG_STOP;
        }
    }
}



RubberBandTracker* TrackerStack::GetRubberBand()
{
    for (auto item : Buffer) {
        if (item->GetType() == TrackerBase::EType::RubberBand) {
            return static_cast<RubberBandTracker*>(item);
        }
    }

    return NULL;
}
