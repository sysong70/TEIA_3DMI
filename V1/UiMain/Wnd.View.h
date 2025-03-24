#pragma once

#include "Cmd.h"
#include "Ctl.ToolBar.h"
#include "Ctl.TabWnd.h"
#include "Wnd.h"

namespace Signal
{
	class Delivery;
}

class ClsPanelManager;
class SgnDelivery2d;
class WndDocument;

//--------------------------------------------------------------------------------------------------

class WndView : public CView
{
public:

	Wnd::EViewType ViewType = Wnd::EViewType::Unknown;
	int ViewId = -1;
	bool ValidRenderer = false;
	bool Activated = false;
	CPoint MousePoint;

	CtlToolBar ToolBarCtl;
	CtlHistoryBar HistoryBarCtl;
	CmdBase* ActiveCommand = nullptr;

public:

	WndView();

	~WndView() override;

	virtual void Activate(bool value) { DEBUG_STOP; }

	virtual void CancelCommand() { DEBUG_STOP; }

	virtual void CreateHistoryBar(Ctl::EPivot pivot = Ctl::EPivot::BottomCenter);

	virtual void CreateToolBar();

	virtual SgnDelivery2d* GetDelivery2d() { RETURN_NULL; }

	virtual Signal::Delivery* GetDelivery3d() { RETURN_NULL; }

	virtual ClsPanelManager* GetPanelManager() { RETURN_NULL; }

	virtual bool IsValid() { RETURN_FALSE; }

	virtual void ReceiveSignal(Json::Object* pData) { DEBUG_STOP; }

public:

	CRect GetClientArea();

	CSize GetClientSize();

	WndDocument* GetDocument() const;

public:

	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	// WARNING - abstract
	void OnDraw(CDC* pDC) override {}

#ifdef _DEBUG
	void AssertValid() const override;

	void Dump(CDumpContext& dc) const override;
#endif

	afx_msg LRESULT OnSignal(WPARAM wp, LPARAM lp);

	DECLARE_DYNCREATE(WndView)
	DECLARE_MESSAGE_MAP()
};
