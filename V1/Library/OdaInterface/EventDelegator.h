#pragma once

#include "WorkerThread.h"
#include "SignalArgs.h"

//--------------------------------------------------------------------------------------------------

class EventDelegator : public WorkerThread
{
public:

    EventDelegator();

    virtual ~EventDelegator();

public:

    virtual bool PostSignal(int type, int id, SignalArgs::Base* pSignal);

    virtual bool PostSignal(SignalArgs::Base* pSignal);
    // WARING - do not lock thread, push only (singal in signal)
    void PushSignal(SignalArgs::Base* pSignal);

    void SendSignal(SignalArgs::Base* pSignal);

protected: // WorkerThread

    bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

protected:

    virtual bool OnClose() { return false; }

    virtual bool OnCommand(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnContextCommand(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnKeyDown(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnInitialize(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnLButtonDown(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnLButtonUp(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnMButtonDown(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnMButtonUp(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnRButtonDown(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnRButtonUp(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnMouseMove(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnMouseWheel(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnPaint(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnResize(SignalArgs::Base* pSignal) { return false; }

    virtual bool OnInput(SignalArgs::Base* pSignal) { return false; }
};
