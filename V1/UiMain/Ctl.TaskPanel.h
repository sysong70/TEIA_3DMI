#pragma once

#include "Ctl.h"
#include "Ctl.PropList.h"
#include "Ctl.TaskBar.h"
#include "Ctl.ToolBar.h"

//#define GetName GetXMLTagName

//--------------------------------------------------------------------------------------------------

class CtlTaskPanel : public CWnd
{
public:

	CString Dictionary;
	CtlToolBar ToolBarCtl;
	CtlPropList PropListCtl;

	int HeaderHeight = 0;
	int FooterHeight = 0;

	UINT CommandId = 0;
	Json::Object* DesignData = nullptr;
	bool AllowAllData = true;

	Json::Object ValueData;
	Json::Object DefaultData;
	bool Initialized = false;

public:

	CtlTaskPanel(UINT commandId, Json::Object* pUiData);

	~CtlTaskPanel() override { DestroyWindow(); }

public:

	bool Initialize(CWnd* pParentWnd);

public:

	virtual void ConstructBody();

	virtual void ConstructFooter() {}

	virtual void ConstructHeader() {}

	virtual void OnReset(CBCGPProp* pProp);

public:

	virtual void ReceiveSignal(Json::Object* pData) { DEBUG_STOP; }

	virtual void SetData(Json::Object& data);

	virtual void SetDefaultData(Json::Object& data) { DefaultData = data; }

public:

	CSize GetSize();

	CString GetTitle();

	Json::Object& GetUiData() { return *DesignData; }

	CtlTaskBar& GetTaskBar();

public:

	afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp) { RETURN(0); }

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
