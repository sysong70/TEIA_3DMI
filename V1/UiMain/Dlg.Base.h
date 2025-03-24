#pragma once

#include "Dlg.h"

//--------------------------------------------------------------------------------------------------

class DlgBase : public CBCGPDialog
{
public:

	// Dialog tamplate id
	UINT TemplateId = IDD_DMI_STANDARD;
	CSize WindowSize;
	int HeaderHeight = 0;
	int FooterHeight = 0;
	// For OnGetMinMaxInfo event
	MINMAXINFO SizeLimit = {};

public:

	DlgBase(UINT nIDTemplate, CWnd* pParent = nullptr)
		: CBCGPDialog(nIDTemplate, pParent)
		, TemplateId(nIDTemplate)
	{}

	~DlgBase() override {}
	// Create and show window from dialog pointer
	void DoModaless();

public:

	virtual int GetId() { return (int)Dlg::EIndex::Unknown; }

	virtual void ReceiveSignal2d(Json::Object* pData) { DEBUG_STOP; }

	virtual void ReceiveSignal3d(Json::Object* pData) { DEBUG_STOP; }

public:

	virtual void ConstructHeader(const CRect& boundary) {}
	
	virtual void ConstructBody(const CRect& boundary) {}
		
	virtual void ConstructFooter(const CRect& boundary) {}
	// exclude margin
	virtual CRect GetBodyRect();
	// include margin
	virtual CSize GetBodySize();

	virtual bool PumpMessages();

public:

	BOOL OnInitDialog() override;

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()

public: // Window helper

	// call after adding child controls
	CSize AdjustWindowSize(CSize clientSize);

	void EnableParent(bool enable = true);

	CRect GetClientArea();
	//:WAIT
	CFont* GetDefaultFont() { return GetFont(); }
	// gap between client area and window nc area
	CRect GetFramePadding();
	// gap bottom right
	CSize GetFrameThickness() { return GetFramePadding().BottomRight(); }
	// dialog control size 32x32 in resource
	CSize GetScaled32();
	// Min, Max dialog size limit
	void SetSizeLimit(bool bMinLimit, bool bMaxLimit);

	void SetSizeLimit(POINT min, POINT max);
};

//--------------------------------------------------------------------------------------------------

class DlgInstances
{
public:

	std::list<DlgBase*> Buffer;

public:

	DlgInstances();

	~DlgInstances();

public:

	void Add(DlgBase* pValue);

	DlgBase* Get(int id);

	void Remove(int id);
};
