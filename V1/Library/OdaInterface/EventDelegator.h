#pragma once

#include "SignalParams.h"
#include "WorkerThread.h"

#include <memory>

//--------------------------------------------------------------------------------------------------

class EventDelegator : public WorkerThread
{
protected: // WorkerThread

    bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

public:

    EventDelegator();

    virtual ~EventDelegator();

    virtual bool PostSignal(int type, int id, SignalParams* pSignal);

    virtual bool PostSignal(SignalParams* pSignal);
    // WARING - do not lock thread, push only (singal in signal)
    virtual bool PushSignal(SignalParams* pSignal);

    virtual bool SendSignal(SignalParams* pSignal);

protected:

    virtual bool OnClose() { return false; }

    virtual bool OnCommand(SignalParams* pSignal) { return false; }

    virtual bool OnContextCommand(SignalParams* pSignal) { return false; }

    virtual bool OnKeyDown(SignalParams* pSignal) { return false; }

    virtual bool OnInitialize(SignalParams* pSignal) { return false; }

    virtual bool OnInput(SignalParams* pSignal) { return false; }

    virtual bool OnLButtonDown(SignalParams* pSignal) { return false; }

    virtual bool OnLButtonUp(SignalParams* pSignal) { return false; }

    virtual bool OnMButtonDown(SignalParams* pSignal) { return false; }

    virtual bool OnMButtonUp(SignalParams* pSignal) { return false; }

    virtual bool OnRButtonDown(SignalParams* pSignal) { return false; }

    virtual bool OnRButtonUp(SignalParams* pSignal) { return false; }

    virtual bool OnMouseMove(SignalParams* pSignal) { return false; }

    virtual bool OnMouseWheel(SignalParams* pSignal) { return false; }

    virtual bool OnPaint(SignalParams* pSignal) { return false; }

    virtual bool OnResize(SignalParams* pSignal) { return false; }
};
