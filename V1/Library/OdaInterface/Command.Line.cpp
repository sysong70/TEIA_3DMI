#include "stdafx.h"
#include "Commands.h"

#include "DbLine.h"

//**************************************************************************************************

struct LineTracker : public TrackerBase
{
    OdDbLine* pLine = nullptr;
    double Length = 0.0;

    void SetGsView(OdGsView* pView) override
    {
        pView->add(pLine, NULL);
    }

    void UnsetGsView(OdGsView* pView) override
    {
        pView->erase(pLine);

        pLine = nullptr;
        Length = 0.0;
    }

    void SetValue(const OdGePoint3d& value) override
    {
        DEBUG_VALID(pLine);

        if (OdZero(Length)) {
            pLine->setEndPoint(value);
            return;
        }

        OdGePoint3d start = pLine->startPoint();
        OdGeVector3d dic = (value - start).normalize() * Length;
        OdGePoint3d end = start + dic;

        pLine->setEndPoint(end);
    }
};

//--------------------------------------------------------------------------------------------------

void LineCommand::Run(Renderer* pRenderer)
{
    CommandParams params(pRenderer);

    bool finished = false;
    int lines = 0;
    OdGePoint3d firstPoint, startPoint, endPoint;

    while (finished == false) {
        firstPoint = startPoint = endPoint = params.pio->GetPoint(Io::LineFirst);

        while (true) {
            OdDbLinePtr pNewLine = OdDbLine::createObject();
            pNewLine->setDatabaseDefaults(params.pDb);
            pNewLine->setNormal(OdGeVector3d::kZAxis);
            pNewLine->setStartPoint(startPoint);
            pNewLine->setEndPoint(endPoint);

            LineTracker tracker;
            tracker.pLine = pNewLine;

            try {
                endPoint = params.pio->GetPoint(Io::LineNext, lines > 1 ? Io::LineK2 : Io::LineK1, &tracker);
            }
            catch (const OdEdKeyword& keyword) {
                if (keyword.keywordIndex() == 0) {
                    // Undo
                    if (lines == 0) {
                        break;
                    }

                    OdDbObjectId lastId = oddbEntLast(params.pDb);
                    OdDbLinePtr pLine = lastId.openObject(OdDb::kForWrite);

                    if (pLine.isNull() == false) {
                        startPoint = pLine->startPoint();
                        pLine->erase();
                        --lines;
                    }
                    else {
                        DEBUG_STOP;
                    }

                    continue;
                }
                else if (keyword.keywordIndex() == 1) {
                    // Close
                    endPoint = firstPoint;
                    finished = true;
                }
                else {
                    DEBUG_STOP;
                }
            }
            catch (const OdEdCancel&) {
                return;
            }

            pNewLine->setEndPoint(endPoint);
            params.pSpace->appendOdDbEntity(pNewLine);
            startPoint = endPoint;
            ++lines;

            if (finished) {
                return;
            }
        }
    }
}
