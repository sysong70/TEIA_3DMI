#include "stdafx.h"
#include "Trackers.h"

#include "Gi/GiCommonDraw.h"

//**************************************************************************************************

OdUInt32 GraphTracker::subSetAttributes(OdGiDrawableTraits* pDt) const
{
    return kDrawableIsAnEntity;
}



bool GraphTracker::subWorldDraw(OdGiWorldDraw* pWd) const
{
    return false;
}



void GraphTracker::Invalidate()
{
    if (m_pGsModel != nullptr) {
        OdGiDrawable* pParent = nullptr;
        m_pGsModel->onModified(this, pParent);
    }
}



void GraphTracker::Initialize(OdGsView* pView)
{
	if (pView != nullptr && pView->device() != nullptr && m_pGsModel == nullptr) {
	    m_pGsModel = pView->device()->createModel().detach();
		if (m_pGsModel != nullptr) {
			// Skip Z-buffer for 2d drawables.
			m_pGsModel->setRenderType(OdGsModel::kDirect);
		}
	}

    pView->add(this, m_pGsModel);
}



void GraphTracker::Terminate(OdGsView* pView)
{
    pView->erase(this);
}

//**************************************************************************************************

void RubberBand::subViewportDraw(OdGiViewportDraw* pVd) const
{
    OdGeVector3d dir = m_end - m_start;
    double length = dir.normalizeGetLength();

    OdGiDrawFlagsHelper dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
    // CHECK - color
    pVd->subEntityTraits().setColor(40);

    OdGePoint3d segments[2];
    segments[0] = m_start;
    segments[1] = m_end;

    if (length > 0.0) {
        OdGePoint2d pixelDensity;

        pVd->viewport().getNumPixelsInUnitSquare(m_start, pixelDensity, false);
        double dashLength = 20.0 / pixelDensity.x;
        double gapLength = 10.0 / pixelDensity.x;

        OdGiViewportGeometry& geom = pVd->geometry();
        double drawn = 0.0;

        while (drawn < length) {
            segments[1] = segments[0] + dir * dashLength;
            drawn += dashLength;

            if (drawn > length) {
                segments[1] = m_end;
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



void RubberBand::SetBasePoint(const OdGePoint3d& value)
{
    m_start = m_end = value;
}



void RubberBand::SetValue(const OdGePoint3d& value)
{
    m_end = value;
    Invalidate();
}

//**************************************************************************************************

void RubberRect::subViewportDraw(OdGiViewportDraw* pVd) const
{
    OdGePoint3d points[5];
    points[0] = m_start;
    points[1] = { m_end.x, m_start.y, 0 };
    points[2] = m_end;
    points[3] = { m_start.x, m_end.y, 0 };
    points[4] = m_start;

    pVd->geometry().polyline(5, points);
}

//**************************************************************************************************

void TrackerStack::Initialize(OdGsView* pView)
{
    for (auto item : m_buffer) {
        item->Initialize(pView);
    }
}



void TrackerStack::Clear(OdGsView* pView)
{
    for (auto item : m_buffer) {
        item->Terminate(pView);
    }

    m_buffer.clear();
}

//--------------------------------------------------------------------------------------------------

void TrackerStack::SetValue(double value)
{
    for (auto item : m_buffer) {
        item->SetValue(value);
    }
}



void TrackerStack::SetValue(int value)
{
    for (auto item : m_buffer) {
        item->SetValue(value);
    }
}



void TrackerStack::SetValue(const OdGePoint3d& value)
{
    for (auto item : m_buffer) {
        item->SetValue(value);
    }
}



void TrackerStack::SetValue(CString value)
{
    for (auto item : m_buffer) {
        item->SetValue(value);
    }
}

//--------------------------------------------------------------------------------------------------

void TrackerStack::Push(TrackerBase* pTracker)
{
    if (pTracker != nullptr) {
        m_buffer.push_back(pTracker);
    }
}



void TrackerStack::Pop(TrackerBase* pTracker)
{
    if (pTracker != nullptr) {
        if (pTracker == m_buffer.back()) {
            m_buffer.pop_back();
            DEBUG_STOP;
        }
    }
}
