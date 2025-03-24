#pragma once

#include "Dlg.Standard.h"

//--------------------------------------------------------------------------------------------------

class DlgDebugTracer : public DlgStandard
{
public:
	
	// Body controls
	CBCGPListBox LogCtl;

public:

	DlgDebugTracer(CWnd* pParent = nullptr)
		: DlgStandard(IDD_DMI_DEBUG_TRACER, "DebugTracer", pParent)
	{}

	~DlgDebugTracer() override {}

public:

	int GetId() override { return (int)Dlg::EIndex::DebugTracer; }

	void ReceiveSignal2d(Json::Object* pData) override;

public:

	void DoDataExchange(CDataExchange* pDX) override;

	BOOL OnInitDialog() override;

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	void OnContextCommand(UINT id);

	DECLARE_DYNAMIC(DlgDebugTracer)
	DECLARE_MESSAGE_MAP()

public:

	void ConstructBody(const CRect& boundary) override;

public:

	void AddLog(Json::Object& data);
	// local
	void AddLog(const wchar_t* pFormat, ...);

	void AddLog(CString log);

	void ClearLog();

	void SaveLog(Json::Object& data);

	void SaveLog(CString path, bool clear = true);
};
