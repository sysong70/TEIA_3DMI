#pragma once

#include "Dlg.Base.h"

//--------------------------------------------------------------------------------------------------

class DlgStandard : public DlgBase
{
public:

	// Dialog structure information from TheResource
	Json::Object* DesignData = nullptr;

public:

	// dialogName in Json
	DlgStandard(CStringA dialogName, CWnd* pParent = nullptr);

	DlgStandard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent = nullptr);

	~DlgStandard() override {}

public:

	BOOL OnInitDialog() override;

	DECLARE_DYNAMIC(DlgStandard)
	DECLARE_MESSAGE_MAP()

public:

	Json::Object& GetDefaultButtons();

	Json::Object& GetUiData();

	CSize GetWinSize();

	CString GetWinTitle();
};
