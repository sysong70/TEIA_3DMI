#include "stdafx.h"

#include "Commands.h"

#include "DbCircle.h"
#include "Ge/GeCircArc3d.h"

//**************************************************************************************************

class CircleTracker : public TrackerBase
{
public:

    OdDbCircle* CirclePtr = nullptr;

public:

    CircleTracker(OdDbCircle* pCircle) : CirclePtr(pCircle) {}

    void Initialize(OdGsView* pView) override
    {
        if (Protect == false) {
            pView->add(CirclePtr, 0);
            Protect = true;
        }
    };

    void Terminate(OdGsView* pView) override
    {
        if (Protect == false) {
            pView->erase(CirclePtr);
            CirclePtr = nullptr;
        }
    }
};

//--------------------------------------------------------------------------------------------------

class CircleRadiusTracker : public TrackerBase
{
public:

    CircleTracker* ParentPtr = nullptr;
    bool Diameter = false;

public:

    CircleRadiusTracker(CircleTracker* pParent, OdGePoint3d center)
        : ParentPtr(pParent)
    {
        ParentPtr->CirclePtr->setCenter(center);
    }

    void SetValue(double radius) override
    {
        if (Diameter) {
            radius /= 2.0;
        }

        ParentPtr->CirclePtr->setRadius(radius);
    }

    void SetValue(const OdGePoint3d& point) override
    {
        OdGePoint3d center = ParentPtr->CirclePtr->center();
        double radius = (center - point).length();
        SetValue(radius);
    }

    void Initialize(OdGsView* pView) override
    {
        ParentPtr->Initialize(pView);
    }

    void Terminate(OdGsView* pView) override
    {
        ParentPtr->Terminate(pView);
    }
};



static void TrackRadius(UserIO& io, OdGePoint3d centerPoint, CircleTracker* pTracker)
{
    CircleRadiusTracker tracker(pTracker, centerPoint);

    // Read radius point
    try {
        OdGePoint3d point = io.GetPoint(
            Uio::CircleCrRadius,
            Uio::eUseLengthGuideOnly | Uio::eOtherPoint,
            Uio::CircleCrK2,
            &tracker
        );
        tracker.SetValue(point);
        return;
    }
    catch (const OdEdKeyword& keyword) {
        ASSERT(keyword.keywordIndex() == 0);
    }

    // Read diameter value or radius point
    try {
        OdGePoint3d point = io.GetPoint(
            Uio::CircleCrDiameter,
            Uio::eUseLengthGuideOnly | Uio::eAllowReal | Uio::eOtherPoint,
            nullptr,
            &tracker
        );
        tracker.SetValue(point);
    }
    catch (const UioResult& result) {
        ASSERT(result.Real > 0.0);
        tracker.SetValue(result.Real);
    }
}

//--------------------------------------------------------------------------------------------------

class Circle3pTracker : public TrackerBase
{
public:

    OdGePoint3d Point1 = OdGePoint3d::kOrigin;
    OdGePoint3d Point2 = OdGePoint3d::kOrigin;
    CircleTracker* ParentPtr = nullptr;

public:

    Circle3pTracker(CircleTracker* pParent) : ParentPtr(pParent) {}

    void SetValue(const OdGePoint3d& point3) override
    {
        OdGeCircArc3d geArc;

        // Check if arc changed direction. Normal should not be flipped.
        OdGeVector3d v1 = Point2 - Point1;
        OdGeVector3d v2 = point3 - Point2;
        OdGeVector3d vNewNormal = v1.crossProduct(v2);
        if (vNewNormal == OdGeVector3d::kIdentity) {
            return;
        }

        if (vNewNormal.isCodirectionalTo(ParentPtr->CirclePtr->normal())) {
            geArc.set(Point1, Point2, point3);
        }
        else {
            geArc.set(point3, Point2, Point1);
        }

        geArc.setAngles(0.0, Oda2PI);
        ParentPtr->CirclePtr->setFromOdGeCurve(geArc);
    }

    void Initialize(OdGsView* pView) override
    {
        return ParentPtr->Initialize(pView);
    }

    void Terminate(OdGsView* pView) override
    {
        ParentPtr->Terminate(pView);
    }
};



static void Track3Points(UserIO& io, CircleTracker* pTracker)
{
    Circle3pTracker tracker(pTracker);

    tracker.Point1 = io.GetPoint(Uio::Circle3pFirst, Uio::eFirstPoint);
    tracker.Point2 = io.GetPoint(Uio::Circle3pSecond, Uio::eUseBandGuide | Uio::eOtherPoint);
    OdGePoint3d point = io.GetPoint(Uio::Circle3pThird, Uio::eUseBandGuide | Uio::eOtherPoint, nullptr, &tracker);
    tracker.SetValue(point);
}

//--------------------------------------------------------------------------------------------------

class Circle2pTracker : public TrackerBase
{
public:

    OdGePoint3d Point1 = OdGePoint3d::kOrigin;
    CircleTracker* ParentPtr = nullptr;

    Circle2pTracker(CircleTracker* pParent) : ParentPtr(pParent) {}

    void SetValue(const OdGePoint3d& point2) override
    {
        OdGeVector3d centerVector = (point2 - Point1) / 2.0;
        double dRadius = centerVector.length();
        if (OdZero(dRadius) == false) {
            ParentPtr->CirclePtr->setCenter(Point1 + centerVector);
            ParentPtr->CirclePtr->setRadius(centerVector.length());
        }
    }

    void Initialize(OdGsView* pView)
    {
        ParentPtr->Initialize(pView);
    }

    void Terminate(OdGsView* pView)
    {
        ParentPtr->Terminate(pView);
    }
};



static void Track2Points(UserIO& io, CircleTracker* pTracker)
{
    Circle2pTracker tracker(pTracker);

    tracker.Point1 = io.GetPoint(Uio::Circle2pFirst, Uio::eFirstPoint);
    OdGePoint3d point = io.GetPoint(Uio::Circle2PSecond, Uio::eUseBandGuide | Uio::eOtherPoint, nullptr, &tracker);
    tracker.SetValue(point);
}

//--------------------------------------------------------------------------------------------------

static void TrackTTR(UserIO& io, CircleTracker* pTracker)
{
    DEBUG_STOP;
}

//**************************************************************************************************

void CmdCircle::Run(Renderer* pRenderer)
{
    CommandParams params(Name(), pRenderer);

    OdDbCirclePtr pNewCircle = OdDbCircle::createObject();
    pNewCircle->setDatabaseDefaults(params.DbPtr);
    pNewCircle->setNormal(OdGeVector3d::kZAxis);

    int nKeyword = -1;
    OdGePoint3d centerPoint;

    CircleTracker tracker(pNewCircle);
    // WARNING - lock process
    tracker.Initialize(params.GsViewPtr);

    try {
        centerPoint = params.IoPtr->GetPoint(
            Uio::CircleCrCenter,
            Uio::eFirstPoint,
            Uio::CircleCrK1
        );
    }
    catch (const OdEdKeyword& keyword) {
        nKeyword = keyword.keywordIndex();
        ASSERT(nKeyword > -1);
    }
    catch (const OdEdCancel&) {
        goto Exit;
    }

    try {
        switch (nKeyword) {
        case -1: TrackRadius(*params.IoPtr, centerPoint, &tracker); break;
        case 0:  Track3Points(*params.IoPtr, &tracker); break;
        case 1:  Track2Points(*params.IoPtr, &tracker); break;
        case 2:  TrackTTR(*params.IoPtr, &tracker); break;

        default:
            DEBUG_STOP;
        }
    }
    catch (const OdEdCancel&) {
        goto Exit;
    }

    params.SpacePtr->appendOdDbEntity(pNewCircle);
    params.Completed = true;

Exit:

    // WARNING - disable Protect first
    tracker.Protect = false;
    tracker.Terminate(params.GsViewPtr);
}
