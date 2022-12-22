#pragma once

#include <map>

class DmiErrorHandler : public HPS::EventHandler
{
public:
	DmiErrorHandler() : HPS::EventHandler() {}
	virtual ~DmiErrorHandler() { Shutdown(); }

	// Override to provide behavior for an error event
	virtual HandleResult Handle(HPS::Event const * pcEvent)
	{
		//ASSERT(pcEvent != NULL);
		HPS::ErrorEvent const * error = static_cast<HPS::ErrorEvent const *>(pcEvent);
		HPS::UTF8 msg = HPS::UTF8("Error: ") + error->message + HPS::UTF8("\n");
		HPS::WCharArray wmsg;
		msg.ToWStr(wmsg);
		OutputDebugString(&wmsg[0]);
		return HandleResult::Handled;
	}
};


// Class to handle warnings
class DmiWarningHandler : public HPS::EventHandler
{
public:
	DmiWarningHandler() : HPS::EventHandler() {}
	virtual ~DmiWarningHandler() { Shutdown(); }

	// Override to provide behavior for a warning event
	virtual HandleResult Handle(HPS::Event const * pcEvent)
	{
		// ASSERT(pcEvent != NULL);
		HPS::WarningEvent const * warning = static_cast<HPS::WarningEvent const *>(pcEvent);
		HPS::UTF8 msg = HPS::UTF8("Warning: ") + warning->message + HPS::UTF8("\n");
		HPS::WCharArray wmsg;
		msg.ToWStr(wmsg);
		OutputDebugString(&wmsg[0]);
		return HandleResult::Handled;
	}
};