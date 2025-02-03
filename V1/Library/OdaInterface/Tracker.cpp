#include "stdafx.h"
#include "Tracker.h"
#include "AbstractViewPE.h"
#include "DbBaseDatabase.h"
#include "Ge/GePlane.h"
#include "Renderer.h"

//**************************************************************************************************

int Tracker::Base::addDrawables(OdGsView* pView)
{
	m_pView = pView;
	InitializeGsModel(pView);

	pView->add(this, GetGsModel());
	pView->add(&m_osnapManager, GetGsModel());

	m_bValid = true;

	return 2;
}



void Tracker::Base::removeDrawables(OdGsView* pView)
{
	if (pView == nullptr) {
		if (m_pView == nullptr) {
			return;
		}

		pView = m_pView;
	}

	pView->erase(this);
	pView->erase(&m_osnapManager);

	ResetGsModel();

	m_bValid = false;
}

//--------------------------------------------------------------------------------------------------

OdUInt32 Tracker::Base::subSetAttributes(OdGiDrawableTraits* pDt) const
{
	return kDrawableIsAnEntity;
}



void Tracker::Base::subViewportDraw(OdGiViewportDraw* pVd) const
{
}



bool Tracker::Base::subWorldDraw(OdGiWorldDraw* pWd) const
{
	return false;
}

//--------------------------------------------------------------------------------------------------

void Tracker::Base::Initialize(const OdGePoint3d& base, int pointMax)
{
	ClearValues();

	m_nPointMax = pointMax;

	for (UINT i = 0; i < m_nPointMax; i++) {
		m_points.append(base);
	}

	if (m_nPointMax == 0) {
		m_points.append(base);
	}
}



void Tracker::Base::SetValue(UINT index, const OdGePoint3d& point, bool lastPoint)
{
	if (index < m_nPointMax) {
		m_nPointIndex = index;
		m_points[index] = point;
		if (lastPoint) {
			m_pLastPoint = &(m_lastPoint = point);
		}
	}

	InvalidateGsModel();
}

//--------------------------------------------------------------------------------------------------

OdGsModel* Tracker::Base::GetGsModel() const
{
	return const_cast<OdGsModel*>(m_pModel.get());
}



bool Tracker::Base::HasGsModel() const
{
	return m_pModel.isNull() == false;
}



void Tracker::Base::InitializeGsModel(OdGsView* pView, bool force)
{
	if (pView != nullptr && (force || HasGsModel() == false) && pView->device() != nullptr) {
		OdGsModelPtr pModel = pView->device()->createModel();
		SetGsModel(pModel.get());
		if (HasGsModel()) {
			// Skip Z-buffer for 2d drawables.
			GetGsModel()->setRenderType(OdGsModel::kDirect);
			/*
			// Skip extents calculation.
			GetGsModel()->setEnableViewExtentsCalculation(false);
			// Setup 2dWireframe mode for all underlying geometry.
			GetGsModel()->setRenderModeOverride(OdGsView::k2DOptimized);

			OdDbStub* visualStyleId = GetVisualStyleOverride(pView->userGiContext()->database());
			if (visualStyleId != nullptr) {
				// 2dWireframe visual style.
				GetGsModel()->setVisualStyle(visualStyleId);
			}
			*/
		}
	}
}



void Tracker::Base::InvalidateGsModel()
{
	// CHECK
	//OdGiDrawable* pParent = NULL;

	if (HasGsModel()) {
		GetGsModel()->onModified(this, (OdGiDrawable*)nullptr);
		GetGsModel()->onModified(&m_osnapManager, (OdGiDrawable*)nullptr);
	}
}



void Tracker::Base::ResetGsModel()
{
	m_pModel.release();
}



void Tracker::Base::SetGsModel(OdGsModel* pModel)
{
	m_pModel = pModel;
}



OdDbStub* Tracker::Base::GetVisualStyleOverride(OdDbBaseDatabase* pDb)
{
	OdDbBaseDatabasePEPtr pDbPE(pDb);

	if (pDbPE.isNull() == false) {
		OdRxIteratorPtr pIt = pDbPE->visualStyles(pDb);

		if (pIt.isNull() == false) {
			while (pIt->done() == false) {
				OdDbBaseVisualStylePEPtr pVSPE = pIt->object();
				if (pVSPE.isNull() == false) {
					if (pVSPE->name(pIt->object()) == OD_T("2dWireframe")) {
						return pDbPE->getId(pIt->object());
					}
				}
				pIt->next();
			}
		}
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

UINT Tracker::Base::AddValue(const OdGePoint3d& point)
{
	ASSERT(m_nPointMax == 0);
	m_nPointIndex++;

	return m_points.append(point);
}



void Tracker::Base::ClearValues()
{
	m_points.clear();
	m_nPointMax = 0;
	m_nPointIndex = 0;
	m_pLastPoint = nullptr;
}



UINT Tracker::Base::GetPointIndex()
{
	return m_nPointIndex;
}



OdGePoint3d Tracker::Base::GetValue(UINT index)
{
	DEBUG(0 <= index && index < m_points.length());
	return m_points[index];
}



OdGePoint3dArray& Tracker::Base::GetValues()
{
	return m_points;
}



bool Tracker::Base::IsValid()
{
	return m_bValid;
}



void Tracker::Base::IsValid(bool value)
{
	m_bValid = value;
}



bool Tracker::Base::Snap(OdGePoint3d& point)
{
	//if (!theSetting.theMain.OSnap.Enable) {
	//	return false;
	//}

	//if (m_oSnapManager.Snap(m_pView, point, m_pLastPoint)) {
	//	InvalidateGsModel();
	//	return true;
	//}

	return false;
}

//**************************************************************************************************

OdEdInputTrackerPtr Tracker::Line::Create()
{
	return OdRxObjectImpl<Line, OdEdInputTracker>::createObject();
}



OdEdInputTrackerPtr Tracker::Line::Create(const OdGePoint3d& base, Renderer* pRenderer)
{
	OdEdInputTrackerPtr pResult = OdRxObjectImpl<Line, OdEdInputTracker>::createObject();

	Line* pTracker = static_cast<Line*>(pResult.get());
	pTracker->addDrawables(pRenderer->GetGsView());
	pTracker->Initialize(base, 2);

	return pResult;
}



void Tracker::Line::subViewportDraw(OdGiViewportDraw* pVd) const
{
	if (m_nPointIndex == 0) {
		return;
	}

	pVd->geometry().polyline(m_nPointIndex + 1, m_points.asArrayPtr());
}

//**************************************************************************************************

OdEdInputTrackerPtr Tracker::Polyline::Create()
{
	return OdRxObjectImpl<Polyline, OdEdInputTracker>::createObject();
}



OdEdInputTrackerPtr Tracker::Polyline::Create(const OdGePoint3d& base, Renderer* pRenderer)
{
	OdEdInputTrackerPtr pResult = OdRxObjectImpl<Polyline, OdEdInputTracker>::createObject();

	Polyline* pTracker = static_cast<Polyline*>(pResult.get());
	pTracker->addDrawables(pRenderer->GetGsView());
	pTracker->Initialize(base);

	return pResult;
}



void Tracker::Polyline::subViewportDraw(OdGiViewportDraw* pVd) const
{
	if (m_nPointIndex == 0) {
		return;
	}

	pVd->geometry().polyline(m_nPointIndex + 1, m_points.asArrayPtr());
}

//**************************************************************************************************

OdEdInputTrackerPtr Tracker::RubberBand::Create(const OdGePoint3d& base, Renderer* pRenderer)
{
	OdEdInputTrackerPtr pResult = OdRxObjectImpl<RubberBand, OdEdInputTracker>::createObject();

	RubberBand* pTracker = static_cast<RubberBand*>(pResult.get());
	//pTracker->addDrawables(pRenderer->GetGsView());
	pTracker->Initialize(base, 2);

	return pResult;
}



void Tracker::RubberBand::subViewportDraw(OdGiViewportDraw* pVd) const
{
	OdGiDrawFlagsHelper dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
	pVd->subEntityTraits().setColor(40);

	OdGeVector3d dir = m_points[1] - m_points[0];
	double length = dir.normalizeGetLength();

	if (length > 0.0) {
		OdGePoint2d pixelDensity;
		pVd->viewport().getNumPixelsInUnitSquare(m_points[0], pixelDensity, false);
		double dashLen = 20.0 / pixelDensity.x;
		double gapLen = 10.0 / pixelDensity.x;

		OdGePoint3d segments[2];
		segments[0] = m_points[0];

		OdGiViewportGeometry& geom = pVd->geometry();
		// Draw dash line
		for (double drawn = 0.0; drawn < length;) {
			segments[1] = segments[0] + dir * dashLen;
			drawn += dashLen;

			if (drawn > length) {
				segments[1] = m_points[1];
				geom.polyline(2, segments);
				break;
			}

			geom.polyline(2, segments);
			segments[0] = segments[1] + dir * gapLen;
			drawn += gapLen;
		}
	}
	else {
		pVd->geometry().polyline(2, m_points.asArrayPtr());
	}
}

//**************************************************************************************************

OdEdInputTrackerPtr Tracker::RectFrame::Create(const OdGePoint3d& base, Renderer* pRenderer)
{
	OdEdInputTrackerPtr pResult = OdRxObjectImpl<RectFrame, OdEdInputTracker>::createObject();

	RectFrame* pTracker = static_cast<RectFrame*>(pResult.get());
	//pTracker->addDrawables(pRenderer->GetGsView());
	pTracker->Initialize(base, 4);

	return pResult;
}



OdUInt32 Tracker::RectFrame::subSetAttributes(OdGiDrawableTraits* pDt) const
{
	OdGiSubEntityTraitsPtr pTraits = OdGiSubEntityTraits::cast(pDt);
	if (pTraits.isNull() == false) {
		pTraits->setFillType(kOdGiFillNever);
	}

	return kDrawableIsAnEntity;
}



void Tracker::RectFrame::subViewportDraw(OdGiViewportDraw* pVd) const
{
	const OdGiViewport& vp = pVd->viewport();
	OdGeMatrix3d mat = vp.getWorldToEyeTransform();
	OdGePoint3d p0 = mat * m_points[0];
	OdGePoint3d p2 = mat * m_points[2];

	m_points[1].x = p0.x;
	m_points[3].x = p2.x;
	m_points[1].y = p2.y;
	m_points[3].y = p0.y;
	m_points[1].z = m_points[3].z = 0.0;

	mat = vp.getEyeToWorldTransform();
	m_points[1].transformBy(mat);
	m_points[3].transformBy(mat);

	if (HasGsModel()) {
		// fix invalid plane clipping of selection rectangle for some DGN drawing (like samp4.dgn)
		bool clipFront = false;
		bool clipBack = false;
		double front, back;

		if (vp.getFrontAndBackClipValues(clipFront, clipBack, front, back) && clipFront && clipBack) {
			OdGePoint3d ptBase = vp.getCameraLocation();
			ptBase += vp.viewDir().normalize() * ((front + back) / 2.0);
			OdGePlane plane(ptBase, vp.viewDir());
			for (int index = 0; index < 4; index++) {
				if (!plane.isOn(m_points[index])) {
					OdGePoint3d projPoint;
					if (plane.project(m_points[index], projPoint)) {
						m_points[index] = projPoint;
					}
				}
			}
		}
	}

	OdGiDrawFlagsHelper dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
	pVd->geometry().polygon(4, m_points.asArrayPtr());
}

//**************************************************************************************************

OdEdInputTrackerPtr Tracker::Combo::Create(const OdGePoint3d& base, Renderer* pRenderer)
{
	return OdRxObjectImpl<Combo, OdEdInputTracker>::createObject();
}



int Tracker::Combo::addDrawables(OdGsView* pView)
{
	return First->addDrawables(pView) + Second->addDrawables(pView);
}



void Tracker::Combo::removeDrawables(OdGsView* pView)
{
	First->removeDrawables(pView);
	Second->removeDrawables(pView);
}



void Tracker::Combo::SetValue(UINT index, const OdGePoint3d& point, bool lastPoint)
{
	static_cast<Tracker::Base*>(First.get())->SetValue(index, point, lastPoint);
	static_cast<Tracker::Base*>(Second.get())->SetValue(index, point, lastPoint);
}
