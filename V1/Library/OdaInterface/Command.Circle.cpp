#include "stdafx.h"
#include "Commands.h"

#include "DbCircle.h"
#include "Ge/GeCircArc3d.h"

//**************************************************************************************************

struct CircleTracker : public TrackerBase
{
    OdDbCircle* pCircle = nullptr;

    void SetGsView(OdGsView* pView) override
    {
        pView->add(pCircle, 0);
    };

    void UnsetGsView(OdGsView* pView) override
    {
        pView->erase(pCircle);

        pCircle = nullptr;
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

        void SetGsView(OdGsView* pView) override
        {
            pTracker->SetGsView(pView);
        }

        void UnsetGsView(OdGsView* pView) override
        {
            pTracker->UnsetGsView(pView);
        }
    }
    tracker;
    tracker.pTracker = pTracker;

    pTracker->pCircle->setCenter(centerPoint);
    double radius = 1.0;

    CString prompt = Io::CircleCrRadius;
    CString key = Io::CircleCrK2;

Repeat:
    try {
        radius = io.GetDistance(prompt, key, &tracker);
        tracker.SetValue(radius);
    }
    catch (const OdEdKeyword& keyword) {
        ASSERT(keyword.keywordIndex() == 0);
 
        tracker.Diameter = true;
        prompt = Io::CircleCrDiameter;
        key.Empty();

        goto Repeat;
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
                DEBUG_RETURN;
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

        void SetGsView(OdGsView* pView) override
        {
            return pTracker->SetGsView(pView);
        }

        void UnsetGsView(OdGsView* pView) override
        {
            pTracker->UnsetGsView(pView);
        }
    }
    tracker;
    tracker.pTracker = pTracker;

    tracker.Point1 = io.GetPoint(Io::Circle3pFirst);
    tracker.Point2 = io.GetPoint(Io::Circle3pSecond);
    OdGePoint3d point = io.GetPoint(Io::Circle3pThird, L"", &tracker);
    tracker.SetValue(point);
}



static void Track2Points(UserIO& io, CircleTracker* pTracker)
{
    struct Circle2Points : TrackerBase
    {
        OdGePoint3d Point1 = OdGePoint3d::kOrigin;
        CircleTracker* pTracker = nullptr;

        void SetPoint(const OdGePoint3d& point2)
        {
            OdGeVector3d centerVector = (point2 - Point1) / 2.0;
            double dRadius = centerVector.length();
            if (OdZero(dRadius) == false) {
                pTracker->pCircle->setCenter(Point1 + centerVector);
                pTracker->pCircle->setRadius(centerVector.length());
            }
        }

        void SetGsView(OdGsView* pView)
        {
            pTracker->SetGsView(pView);
        }

        void UnsetGsView(OdGsView* pView)
        {
            pTracker->UnsetGsView(pView);
        }
    }
    tracker;

    tracker.Point1 = io.GetPoint(Io::Circle2pFirst);
    OdGePoint3d point = io.GetPoint(Io::Circle2PSecond, L"", &tracker);
    tracker.SetValue(point);
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

    try {
        centerPoint = params.pio->GetPoint(Io::CircleCrCenter, Io::CircleCrK1);
    }
    catch (const OdEdKeyword& keyword) {
        nKeyword = keyword.keywordIndex();
        ASSERT(nKeyword > -1);
    }
    catch (const OdEdCancel&) {
        params.Cancel();
    }

    try {
        switch (nKeyword) {
            case -1: TrackRadius(*params.pio, centerPoint, &tracker); break;
            case 0:  Track3Points(*params.pio, &tracker); break;
            case 1:  Track2Points(*params.pio, &tracker); break;

            default:
                DEBUG_STOP;
        }
    }
    catch (const OdEdCancel&) {
        params.Cancel();
    }

    params.pSpace->appendOdDbEntity(pNewCircle);
}
