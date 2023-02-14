#pragma once

#include "TD_PackPush.h"

#include "Ed/EdFunctionIO.h"
#include "SSet.h"
#include "StaticRxObject.h"
#include "ExEdBaseIO.h"
#include "Ge/GePlane.h"
#include "Ge/GeLine2d.h"
#include "Ge/GeLineSeg2d.h"
#include "Gi/GiUtils.h"
#include "CmColorBase.h"
#include "RxObjectImpl.h"
#include "Ge/GeLineSeg3d.h"
#include "Gi/GiWorldDraw.h"

//--------------------------------------------------------------------------------------------------

class GraphTrackerBase : public OdEdPointDefTracker, public OdGiDrawableImpl<>
{
protected:

	ODRX_USING_HEAP_OPERATORS(OdEdPointDefTracker);

	OdGsModelPtr m_pModel;

	GraphTrackerBase() { }

	bool hasGsModel() const
	{
		return !m_pModel.isNull();
	}

	OdGsModel* gsModel() const
	{
		return const_cast<OdGsModel*>(m_pModel.get());
	}

	void setGsModel(OdGsModel* pModel)
	{
		m_pModel = pModel;
	}

	void resetGsModel()
	{
		m_pModel.release();
	}

	void initializeGsModel(OdGsView* pView, bool bForce = false)
	{
		if (pView && (bForce || hasGsModel() == false) && pView->device()) {
			OdGsModelPtr pModel = pView->device()->createModel();
			setGsModel(pModel.get());
			if (hasGsModel()) {
				gsModel()->setRenderType(OdGsModel::kDirect); // Skip Z-buffer for 2d drawables.
				gsModel()->setEnableViewExtentsCalculation(false); // Skip extents calculation.
				gsModel()->setRenderModeOverride(OdGsView::k2DOptimized); // Setup 2dWireframe mode for all underlying geometry.
				OdDbStub* visualStyleId = getVisualStyleOverride(pView->userGiContext()->database());
				if (visualStyleId) {
					gsModel()->setVisualStyle(visualStyleId); // 2dWireframe visual style.
				}
			}
		}
	}

public:

	static OdDbStub* getVisualStyleOverride(OdDbBaseDatabase* pDb)
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

	void invalidateGsModel()
	{
		OdGiDrawable* pParent = nullptr;
		if (hasGsModel()) {
			gsModel()->onModified(this, pParent);
		}
	}

	int addDrawables(OdGsView* pView)
	{
		initializeGsModel(pView);
		pView->add(this, gsModel());

		return 1;
	}

	void removeDrawables(OdGsView* pView)
	{
		pView->erase(this);
	}
};

//--------------------------------------------------------------------------------------------------

class PolylineSelectionTracker : public GraphTrackerBase
{
protected:

	ODRX_USING_HEAP_OPERATORS(GraphTrackerBase);

	const OdGePoint3dArray* m_pts;
	OdGePoint3d m_pt;

	void setValue(const OdGePoint3d& pt)
	{
		m_pt = pt;
		invalidateGsModel();
	}

	void setValue(const OdGePoint3dArray* pts)
	{
		m_pts = pts;
	}

	virtual OdUInt32 subSetAttributes(OdGiDrawableTraits*) const ODRX_OVERRIDE
	{
		return kDrawableIsAnEntity;
	}

	virtual bool subWorldDraw(OdGiWorldDraw* pWd) const ODRX_OVERRIDE
	{
		OdGiDrawFlagsHelper _dfh(pWd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
		if (m_pts && m_pts->size() > 1) {
			pWd->geometry().polyline((OdInt32)m_pts->size(), m_pts->asArrayPtr());
		}
		if (m_pts && m_pts->size() > 0) {
			OdGePoint3d tailPts[2] = { m_pts->last(), m_pt };
			pWd->geometry().polyline(2, tailPts);
		}

		return true;
	}

	OdGePoint3d basePoint() const
	{
		if (m_pts && !m_pts->empty()) {
			return m_pts->first();
		}

		return OdGePoint3d::kOrigin;
	}

public:

	static OdEdPointTrackerPtr create(const OdGePoint3dArray* pts = nullptr)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<PolylineSelectionTracker, OdEdPointTracker>::createObject();
		static_cast<PolylineSelectionTracker*>(pRes.get())->setValue(pts);

		return pRes;
	}
};

//--------------------------------------------------------------------------------------------------

class PolygonSelectionTracker : public PolylineSelectionTracker
{
protected:

	bool subWorldDraw(OdGiWorldDraw* pWd) const
	{
		OdGiDrawFlagsHelper _dfh(pWd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
		PolylineSelectionTracker::subWorldDraw(pWd);
		// Polygon segments must not intersect. This check must be implemented in selection command.
		if (m_pts && m_pts->size() > 1) {
			OdGePoint3d closePts[2] = { m_pt, m_pts->first() };
			pWd->geometry().polyline(2, closePts);
		}

		return true;
	}

public:

	static OdEdPointTrackerPtr create(const OdGePoint3dArray* pts = nullptr)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<PolygonSelectionTracker, OdEdPointTracker>::createObject();
		static_cast<PolygonSelectionTracker*>(pRes.get())->setValue(pts);

		return pRes;
	}

	static bool doesPolySegmentsNotIntersects(const OdGePoint3dArray& segments, const OdGePoint3d& newPoint)
	{
		if (segments.size() < 3) {
			return true;
		}

		OdGePoint3dArray poly = segments;
		OdGeVector3d zAxis = ::odgiFaceNormal(poly.size(), poly.asArrayPtr());
		if (OdZero(zAxis.length())) {
			return true; // All segments are parallel
		}
		poly.push_back(newPoint);
		OdUInt32 nSeg;
		OdGeVector3d xAxis;
		for (nSeg = 0; nSeg < poly.size() - 1; nSeg++) {
			xAxis = segments[nSeg + 1] - segments[nSeg];
			OdGe::ErrorCondition error;
			xAxis.normalize(OdGeContext::gZeroTol, error);
			if (error == OdGe::kOk) {
				break;
			}
		}
		OdGeVector3d yAxis = zAxis.crossProduct(xAxis).normal();
		OdGePoint2dArray poly2d;
		poly2d.resize(poly.size());
		for (nSeg = 0; nSeg < poly.size(); nSeg++) {
			poly2d[nSeg].set(poly[nSeg].asVector().dotProduct(xAxis), poly[nSeg].asVector().dotProduct(yAxis));
		}
		// 1-st segment with all without last
		for (nSeg = 0; nSeg < poly2d.size() - 3; nSeg++) {
			OdGePoint2d rVal;
			if (OdGeLineSeg2d(poly2d[nSeg], poly2d[nSeg + 1]).intersectWith(OdGeLineSeg2d(poly2d[poly2d.size() - 2], poly2d.last()), rVal)) {
				return false;
			}
		}
		// 2-nd segment with all without first
		for (nSeg = 1; nSeg < poly2d.size() - 2; nSeg++) {
			OdGePoint2d rVal;
			if (OdGeLineSeg2d(poly2d[nSeg], poly2d[nSeg + 1]).intersectWith(OdGeLineSeg2d(poly2d.first(), poly2d.last()), rVal)) {
				return false;
			}
		}

		return true;
	}
};

//--------------------------------------------------------------------------------------------------

class PlaneSelectionTracker : public PolylineSelectionTracker
{
protected:

	bool subWorldDraw(OdGiWorldDraw* pWd) const
	{
		OdInt32 nDots = 75;
		OdGiDrawFlagsHelper _dfh(pWd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
		if (m_pts && m_pts->size() > 1) {
			if (m_pts->size() == 3) {
				pWd->geometry().polyline((OdInt32)m_pts->size() - 1, m_pts->asArrayPtr());
			}
			else {
				pWd->geometry().polyline((OdInt32)m_pts->size(), m_pts->asArrayPtr());
			}
		}

		if (m_pts && m_pts->size() > 0 && m_pts->size() < 3) {
			OdGePoint3d tailPts[2] = { m_pts->last(), m_pt };

			// means draw normal
			if (m_pts->size() > 1) {
				OdGiSubEntityTraits& traits = pWd->subEntityTraits();
				traits.setColor(OdCmEntityColor::kACIRed);
				pWd->geometry().rowOfDots(nDots, tailPts[0], (tailPts[1] - tailPts[0]) / double(nDots));
			}
			else {
				pWd->geometry().polyline(2, tailPts);
			}
		}

		// not in drag - only after finising and adding the whole tracker to the storage
		if (m_pts && m_pts->size() == 3) {
			OdGePoint3d tailPts[2] = { (*m_pts)[1], (*m_pts)[2] };

			OdGiSubEntityTraits& traits = pWd->subEntityTraits();
			traits.setColor(OdCmEntityColor::kACIRed);
			pWd->geometry().rowOfDots(nDots, tailPts[0], (tailPts[1] - tailPts[0]) / double(nDots));
		}

		return true;
	}

public:

	static OdEdPointTrackerPtr create(const OdGePoint3dArray* pts = nullptr)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<PlaneSelectionTracker, OdEdPointTracker>::createObject();
		static_cast<PlaneSelectionTracker*>(pRes.get())->setValue(pts);

		return pRes;
	}
};

//--------------------------------------------------------------------------------------------------

class PointToOrientTracker : public OdEdPointTracker
{
	OdGePoint3d m_basePoint;
	OdEdRealTracker* m_pTracker;
	OdGeMatrix3d m_xUcsToWorld;

public:

	static OdEdPointTrackerPtr create(const OdGePoint3d& basePoint, OdEdRealTracker* pTracker)
	{
		OdSmartPtr<PointToOrientTracker> pRes;
		/*
		= OdRxObjectImpl<PointToOrientTracker>::createObject();
		pRes->m_basePoint = basePoint;
		pRes->m_pTracker = pTracker;
		*/
	#pragma MARKMESSAGE(TODO: implement PointToOrientTracker)

		return pRes;
	}

	void setValue(const OdGePoint3d& pt)
	{
		m_pTracker->setValue((pt - m_basePoint).length());
	}

	int addDrawables(OdGsView* pView)
	{
		return m_pTracker->addDrawables(pView);
	}

	void removeDrawables(OdGsView* pView)
	{
		m_pTracker->removeDrawables(pView);
	}
};

//--------------------------------------------------------------------------------------------------

class ANGBASE_Tracker : public OdEdRealTracker
{
	OdEdRealTracker* m_pAngleTracker;
	double m_ANGBASE;

protected:

	ANGBASE_Tracker() {}

public:

	static OdEdRealTrackerPtr create(OdEdRealTracker* pAngleTracker, double ANGBASE)
	{
		OdSmartPtr<ANGBASE_Tracker> pRes = OdRxObjectImpl<ANGBASE_Tracker>::createObject();
		pRes->m_pAngleTracker = pAngleTracker;
		pRes->m_ANGBASE = ANGBASE;

		return pRes;
	}

	void setValue(double orient)
	{
		m_pAngleTracker->setValue(orient - m_ANGBASE);
	}

	int addDrawables(OdGsView* pView)
	{
		return m_pAngleTracker->addDrawables(pView);
	}

	void removeDrawables(OdGsView* pView)
	{
		m_pAngleTracker->removeDrawables(pView);
	}
};

//--------------------------------------------------------------------------------------------------

class RubberBand : public GraphTrackerBase
{
protected:

	ODRX_USING_HEAP_OPERATORS(GraphTrackerBase);

	OdGePoint3d m_pts[2];

	virtual OdUInt32 subSetAttributes(OdGiDrawableTraits*) const ODRX_OVERRIDE
	{
		return kDrawableIsAnEntity;
	}

	virtual bool subWorldDraw(OdGiWorldDraw* pWd) const ODRX_OVERRIDE
	{
		return false;
	}

	virtual void subViewportDraw(OdGiViewportDraw* pVd) const ODRX_OVERRIDE
	{
		OdGeVector3d dir = m_pts[1] - m_pts[0];
		double length = dir.normalizeGetLength();
		if (length > 0.) {
			OdGiDrawFlagsHelper _dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
			OdGePoint3d seg[2];
			pVd->subEntityTraits().setColor(40);
			OdGePoint2d pixDens;
			pVd->viewport().getNumPixelsInUnitSquare(m_pts[0], pixDens, false);
			double dashLen = 20. / pixDens.x;
			double gapLen = 10. / pixDens.x;
			seg[0] = m_pts[0];
			OdGiViewportGeometry& geom = pVd->geometry();
			for (double drawn = 0.; drawn < length;) {
				seg[1] = seg[0] + dir * dashLen;
				drawn += dashLen;
				if (drawn > length) {
					seg[1] = m_pts[1];
					geom.polyline(2, seg);
					break;
				}
				geom.polyline(2, seg);
				seg[0] = seg[1] + dir * gapLen;
				drawn += gapLen;
			}
		}
	}

	OdGePoint3d basePoint() const
	{
		return m_pts[0];
	}

public:

	static OdEdPointTrackerPtr create(const OdGePoint3d& base, OdGsModel* pModel = nullptr)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<RubberBand, OdEdPointTracker>::createObject();
		static_cast<RubberBand*>(pRes.get())->m_pts[0] = base;
		static_cast<RubberBand*>(pRes.get())->m_pts[1] = base; // CORE-17955 When using Draw/Line, the end of the line goes to the side (ODE now refresh view by getPoint for CORE-17773)
		static_cast<RubberBand*>(pRes.get())->setGsModel(pModel);

		return pRes;
	}

	void setValue(const OdGePoint3d& p)
	{
		m_pts[1] = p;
		invalidateGsModel();
	}
};

//--------------------------------------------------------------------------------------------------

class RectFrame : public GraphTrackerBase
{
protected:

	ODRX_USING_HEAP_OPERATORS(GraphTrackerBase);

	mutable OdGePoint3d m_pts[4];

	virtual OdUInt32 subSetAttributes(OdGiDrawableTraits* pDrawTraits) const ODRX_OVERRIDE
	{
		OdGiSubEntityTraitsPtr pTraits = OdGiSubEntityTraits::cast(pDrawTraits);
		if (pTraits.isNull() == false) {
			pTraits->setFillType(kOdGiFillNever);
		}
		return kDrawableIsAnEntity;
	}

	virtual bool subWorldDraw(OdGiWorldDraw* pWd) const ODRX_OVERRIDE
	{
		return false;
	}

	virtual void subViewportDraw(OdGiViewportDraw* pVd) const ODRX_OVERRIDE
	{
		const OdGiViewport& vp = pVd->viewport();
		OdGeMatrix3d x = vp.getWorldToEyeTransform();
		OdGePoint3d p0 = x * m_pts[0];
		OdGePoint3d p2 = x * m_pts[2];

		m_pts[1].x = p0.x;
		m_pts[3].x = p2.x;
		m_pts[1].y = p2.y;
		m_pts[3].y = p0.y;
		m_pts[1].z = m_pts[3].z = p2.z;

		x = vp.getEyeToWorldTransform();
		m_pts[1].transformBy(x);
		m_pts[3].transformBy(x);

		if (hasGsModel()) {
			// fix invalid plane clipping of selection rectangle for some DGN drawing (like samp4.dgn)
			bool clipFront = false;
			bool clipBack = false;
			double front = 0;
			double back = 0;
			if (vp.getFrontAndBackClipValues(clipFront, clipBack, front, back) && clipFront && clipBack) {
				OdGePoint3d ptBase = vp.getCameraLocation();
				ptBase += vp.viewDir().normalize() * ((front + back) / 2.0);
				OdGePlane plane(ptBase, vp.viewDir());
				for (int index = 0; index < 4; index++) {
					if (plane.isOn(m_pts[index]) == false) {
						OdGePoint3d ptProj;
						if (plane.project(m_pts[index], ptProj)) {
							m_pts[index] = ptProj;
						}
					}
				}
			}
		}

		OdGiDrawFlagsHelper _dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
		pVd->geometry().polygon(4, m_pts);
	}

	OdGePoint3d basePoint() const
	{
		return m_pts[0];
	}

public:

	static OdEdPointTrackerPtr create(const OdGePoint3d& base, OdGsModel* pModel)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<RectFrame, OdEdPointTracker>::createObject();
		static_cast<RectFrame*>(pRes.get())->m_pts[0] = base;
		static_cast<RectFrame*>(pRes.get())->m_pts[1] = base;
		static_cast<RectFrame*>(pRes.get())->m_pts[2] = base;
		static_cast<RectFrame*>(pRes.get())->m_pts[3] = base;
		static_cast<RectFrame*>(pRes.get())->setGsModel(pModel);

		return pRes;
	}

	void setValue(const OdGePoint3d& p)
	{
		m_pts[2] = p;
		invalidateGsModel();
	}

	OdGePoint3dArray getPoints() const
	{
		return OdGePoint3dArray::create(m_pts, 4);
	}
};

//--------------------------------------------------------------------------------------------------

class OrthoRubberBand : public GraphTrackerBase
{
	ODRX_USING_HEAP_OPERATORS(GraphTrackerBase);

	mutable OdGePoint3d m_pts[3];
	OdGeLineSeg3d m_prevSegment;

	virtual OdUInt32 subSetAttributes(OdGiDrawableTraits*) const ODRX_OVERRIDE
	{
		return kDrawableIsAnEntity;
	}

	virtual bool subWorldDraw(OdGiWorldDraw* pWd) const ODRX_OVERRIDE
	{
		return false;
	}

	virtual void subViewportDraw(OdGiViewportDraw* pVd) const ODRX_OVERRIDE
	{
		const OdGiViewport& vp = pVd->viewport();
		OdGeMatrix3d x = vp.getWorldToEyeTransform();

		OdGePoint3d pp0 = m_prevSegment.startPoint();
		OdGePoint3d pp1 = m_prevSegment.endPoint();

		OdGePoint2d p0 = (x * m_prevSegment.startPoint()).convert2d();
		OdGePoint2d p1 = (x * m_prevSegment.endPoint()).convert2d();
		OdGePoint2d pToProject = (x * m_pts[2]).convert2d();

		OdGeLineSeg2d prevSegment(p0, p1);
		OdGeLine2d perpLine;
		prevSegment.getPerpLine(p1, perpLine);
		OdGeLine2d perpLineNext;
		perpLine.getPerpLine(pToProject, perpLineNext);

		OdGePoint2d pTargetPoint;
		perpLine.intersectWith(perpLineNext, pTargetPoint);

		x = vp.getEyeToWorldTransform();
		m_pts[0].set(p1.x, p1.y, 0);
		m_pts[0].transformBy(x);
		m_pts[1].set(pTargetPoint.x, pTargetPoint.y, 0);
		m_pts[1].transformBy(x);

		OdGiDrawFlagsHelper _dfh(pVd->subEntityTraits(), OdGiSubEntityTraits::kDrawNoPlotstyle);
		pVd->geometry().polygon(2, m_pts);
	}

	OdGePoint3d basePoint() const
	{
		return m_pts[0];
	}

public:

	/*
	  Creates the tracker with one line segment.

	  base[in] First point of created line segment.
	  nextPoint[in] First point of created line segment.
	  pModel[in] Collections of drawable objects in the ODA Platform framework. See GsModel.h

	  returns the pointer of created tracker.
	*/
	static OdEdPointTrackerPtr create(const OdGePoint3d& base, const OdGePoint3d& nextPoint, OdGsModel* pModel = nullptr)
	{
		OdEdPointTrackerPtr pRes = OdRxObjectImpl<OrthoRubberBand, OdEdPointTracker>::createObject();
		static_cast<OrthoRubberBand*>(pRes.get())->m_pts[0] = base;
		static_cast<OrthoRubberBand*>(pRes.get())->setGsModel(pModel);
		static_cast<OrthoRubberBand*>(pRes.get())->m_prevSegment.set(base, nextPoint);

		return pRes;
	}
	/*
		Creates the tracker with one line segment.

		p[in] The point of perpendicular to drawable segment.
	*/
	void setValue(const OdGePoint3d& p)
	{
		m_pts[2] = p;
		invalidateGsModel();
	}
	/*
		Returns the end point of drawable segment.
	*/
	OdGePoint3d getValue()
	{
		return m_pts[1];
	}
};

#include "TD_PackPop.h"
