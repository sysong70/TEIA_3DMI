#pragma once

#include "Dlg.Standard.h"

//--------------------------------------------------------------------------------------------------

class DlgProgressBar : public DlgStandard
{
public:

	CBCGPStatic MessageCtl;
	CBCGPProgressCtrl ProgressCtl;
	CBCGPListBox LogCtl;

public:

	DlgProgressBar(CWnd* pParent = nullptr)
		: DlgStandard(IDD_DMI_PROGRESS_LOG, "Progress", pParent)
	{}

	~DlgProgressBar() override {}

public:

	int GetId() override { return (int)Dlg::EIndex::ProgressBar; }

	void ReceiveSignal2d(Json::Object* pData) override;

	void ReceiveSignal3d(Json::Object* pData) override;

public:

	BOOL DestroyWindow() override;

	void DoDataExchange(CDataExchange* pDX) override;

	BOOL OnInitDialog() override;

	DECLARE_DYNAMIC(DlgProgressBar)
	DECLARE_MESSAGE_MAP()

public:

	void ConstructBody(const CRect& boundary) override;

public:

	void SetRange(Json::Object& data);

	void SetPosition(Json::Object& data);

	void SetMessage(Json::Object& data);

	void AddLog(Json::Object& data);
	// Last log only
	void SetLogStatus(Json::Object& data);

	void ClearLog();
};

