#pragma once

#include "WorkerThread.h"
#include "SignalArgs.h"



class EventDelegator : public WorkerThread
{
public:

    EventDelegator();

    virtual ~EventDelegator();

public:

    void PostSignal(int type, int id, SignalArgs::Base* signal);

    void PostSignal(SignalArgs::Base* signal);
    //:WARING - do not lock thread, push only (singal in signal)
    void PushSignal(SignalArgs::Base* signal);

    void SendSignal(SignalArgs::Base* signal);

protected:

    bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

    virtual bool OnClose() { return false; }

    virtual void OnCommand(SignalArgs::Command* signal) {};

    virtual bool OnInitialize(SignalArgs::Initialize* signal) { return false; }

    virtual void OnLButtonDown(SignalArgs::Mouse* signal) {}

    virtual void OnLButtonUp(SignalArgs::Mouse* signal) {}

    virtual void OnMButtonDown(SignalArgs::Mouse* signal) {}

    virtual void OnMButtonUp(SignalArgs::Mouse* signal) {}

    virtual void OnRButtonDown(SignalArgs::Mouse* signal) {}

    virtual void OnRButtonUp(SignalArgs::Mouse* signal) {}

    virtual void OnMouseMove(SignalArgs::Mouse* signal) {}

    virtual void OnMouseWheel(SignalArgs::Mouse* signal) {}

    virtual void OnPaint(SignalArgs::Paint* signal) {}

    virtual void OnResize(SignalArgs::Resize* signal) {}

    virtual void OnText(SignalArgs::Text* signal) {}
};
