#include "stdafx.h"
#include "Commands.h"

#include "DbCircle.h"
#include "Ge/GeCircArc3d.h"

//**************************************************************************************************

struct CircleTracker : public TrackerBase
{
    OdDbCircle* pCircle = nullptr;

    void Initialize(OdGsView* pView) override
    {
        if (Protect == false) {
            pView->add(pCircle, 0);
            Protect = true;
        }
    };

    void Terminate(OdGsView* pView) override
    {
        if (Protect == false) {
            pView->erase(pCircle);
            pCircle = nullptr;
        }
    }
};



static void TrackRadius(UserIO& io, OdGePoint3d centerPoint, CircleTracker* pTracker)
{
    struct Radius : TrackerBase
    {
        CircleTracker* pTracker = nullptr;
        bool Diameter = false;

        void SetValue(double radius) override
        {
            if (Diameter) {
                radius /= 2.0;
            }

            pTracker->pCircle->setRadius(radius);
        }

        void SetValue(const OdGePoint3d& point) override
        {
            OdGePoint3d center = pTracker->pCircle->center();
            double radius = (center - point).length();
            SetValue(radius);
        }

        void Initialize(OdGsView* pView) override
        {
            pTracker->Initialize(pView);
        }

        void Terminate(OdGsView* pView) override
        {
            pTracker->Terminate(pView);
        }
    }
    tracker;
    tracker.pTracker = pTracker;

    pTracker->pCircle->setCenter(centerPoint);

    // Read radius point
    try {
        OdGePoint3d point = io.GetPoint(Io::CircleCrRadius, Io::eRubberBand, Io::CircleCrK2, &tracker);
        tracker.SetValue(point);
        return;
    }
    catch (const OdEdKeyword& keyword) {
        ASSERT(keyword.keywordIndex() == 0);
    }

    // Read diameter value or radius point
    try {
        OdGePoint3d point = io.GetPoint(Io::CircleCrDiameter, Io::eAllowReal, nullptr, &tracker);
        tracker.SetValue(point);
    }
    catch (const IoResult& result) {
        ASSERT(result.Real > 0.0);
        tracker.SetValue(result.Real);
    }
}



static void Track3Points(UserIO& io, CircleTracker* pTracker)
{
    struct Circle3Points : TrackerBase
    {
        OdGePoint3d Point1 = OdGePoint3d::kOrigin;
        OdGePoint3d Point2 = OdGePoint3d::kOrigin;
        CircleTracker* pTracker = nullptr;

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

            if (vNewNormal.isCodirectionalTo(pTracker->pCircle->normal())) {
                geArc.set(Point1, Point2, point3);
            }
            else {
                geArc.set(point3, Point2, Point1);
            }

            geArc.setAngles(0.0, Oda2PI);
            pTracker->pCircle->setFromOdGeCurve(geArc);
        }

        void Initialize(OdGsView* pView) override
        {
            return pTracker->Initialize(pView);
        }

        void Terminate(OdGsView* pView) override
        {
            pTracker->Terminate(pView);
        }
    }
    tracker;
    tracker.pTracker = pTracker;

    tracker.Point1 = io.GetPoint(Io::Circle3pFirst, 0);
    tracker.Point2 = io.GetPoint(Io::Circle3pSecond, Io::eRubberBand);
    OdGePoint3d point = io.GetPoint(Io::Circle3pThird, Io::eRubberBand, nullptr, &tracker);
    tracker.SetValue(point);
}



static void Track2Points(UserIO& io, CircleTracker* pTracker)
{
    struct Circle2Points : TrackerBase
    {
        OdGePoint3d Point1 = OdGePoint3d::kOrigin;
        CircleTracker* pTracker = nullptr;

        void SetValue(const OdGePoint3d& point2) override
        {
            OdGeVector3d centerVector = (point2 - Point1) / 2.0;
            double dRadius = centerVector.length();
            if (OdZero(dRadius) == false) {
                pTracker->pCircle->setCenter(Point1 + centerVector);
                pTracker->pCircle->setRadius(centerVector.length());
            }
        }

        void Initialize(OdGsView* pView)
        {
            pTracker->Initialize(pView);
        }

        void Terminate(OdGsView* pView)
        {
            pTracker->Terminate(pView);
        }
    }
    tracker;
    tracker.pTracker = pTracker;

    tracker.Point1 = io.GetPoint(Io::Circle2pFirst, 0);
    OdGePoint3d point = io.GetPoint(Io::Circle2PSecond, Io::eRubberBand, nullptr, &tracker);
    tracker.SetValue(point);
}



static void TrackTTR(UserIO& io, CircleTracker* pTracker)
{
    DEBUG_STOP;
}

//--------------------------------------------------------------------------------------------------

void CircleCommand::Run(Renderer* pRenderer)
{
    CommandParams params(pRenderer);

    OdDbCirclePtr pNewCircle = OdDbCircle::createObject();
    pNewCircle->setDatabaseDefaults(params.pDb);
    pNewCircle->setNormal(OdGeVector3d::kZAxis);

    int nKeyword = -1;
    OdGePoint3d centerPoint;

    CircleTracker tracker;
    tracker.pCircle = pNewCircle;

    // WARNING - lock process
    tracker.Initialize(params.pGsView);

    try {
        centerPoint = params.pIo->GetPoint(Io::CircleCrCenter, 0, Io::CircleCrK1);
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
            case -1: TrackRadius(*params.pIo, centerPoint, &tracker); break;
            case 0:  Track3Points(*params.pIo, &tracker); break;
            case 1:  Track2Points(*params.pIo, &tracker); break;
            case 2:  TrackTTR(*params.pIo, &tracker); break;

            default:
                DEBUG_STOP;
        }
    }
    catch (const OdEdCancel&) {
        goto Exit;
    }

    params.pSpace->appendOdDbEntity(pNewCircle);
    params.Completed = true;

Exit:

    // WARNING - disable Protect first
    tracker.Protect = false;
    tracker.Terminate(params.pGsView);
}
