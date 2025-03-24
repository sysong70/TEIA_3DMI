#include "stdafx.h"

#include "Commands.h"

#include "DbLine.h"

//**************************************************************************************************

class LineTracker : public TrackerBase
{
public:

    OdDbLine* LinePtr = nullptr;

public:

    LineTracker(OdDbLine* pLine) : LinePtr(pLine) {}

    void Initialize(OdGsView* pView) override
    {
        __super::Initialize(pView);
        pView->add(LinePtr, NULL);
    }

    void Terminate(OdGsView* pView) override
    {
        pView->erase(LinePtr);
        LinePtr = nullptr;
    }

    void SetValue(const OdGePoint3d& value) override
    {
        if (LinePtr != nullptr) {
            LinePtr->setEndPoint(value);
        }
    }
};

//**************************************************************************************************

void CmdLine::Run(Renderer* pRenderer)
{
    CommandParams params(Name(), pRenderer);

    bool finished = false;
    int lines = 0;
    OdGePoint3d firstPoint, startPoint, endPoint;

    while (finished == false) {
        try {
            firstPoint = startPoint = endPoint = params.IoPtr->GetPoint(
                Uio::LineFirst,
                Uio::eFirstPoint
            );
        }
        catch (const OdEdCancel&) {
            return;
        }

        while (true) {
            OdDbLinePtr pNewLine = OdDbLine::createObject();
            pNewLine->setDatabaseDefaults(params.DbPtr);
            pNewLine->setNormal(OdGeVector3d::kZAxis);
            pNewLine->setStartPoint(startPoint);
            pNewLine->setEndPoint(endPoint);

            LineTracker tracker(pNewLine);

            try {
                endPoint = params.IoPtr->GetPoint(
                    Uio::LineNext,
                    Uio::eUseGuideOnly | Uio::eOtherPoint,
                    lines > 1 ? Uio::LineK2 : Uio::LineK1,
                    &tracker
                );
            }
            catch (const OdEdKeyword& keyword) {
                // Undo
                if (keyword.keywordIndex() == 0) {
                    if (lines == 0) {
                        finished = true;
                        goto Finished;
                    }

                    OdDbObjectId lastId = oddbEntLast(params.DbPtr);
                    OdDbLinePtr pLine = lastId.openObject(OdDb::kForWrite);

                    if (pLine.isNull() == false) {
                        startPoint = pLine->startPoint();
                        pLine->erase();
                        lines--;
                    }
                    else {
                        DEBUG_STOP;
                    }

                    continue;
                }
                // Close
                else if (keyword.keywordIndex() == 1) {
                    endPoint = firstPoint;
                    finished = true;
                }
                else {
                    DEBUG_STOP;
                }
            }
            catch (const OdEdCancel&) {
                // WARNING - keep the lines created
                params.Completed = true;
                return;
            }

            pNewLine->setEndPoint(endPoint);
            params.SpacePtr->appendOdDbEntity(pNewLine);
            startPoint = endPoint;
            ++lines;
Finished:
            if (finished) {
                params.Completed = true;
                return;
            }
        }
    }
}
