#pragma once

#include "Wnd.h"

class WndView;

//--------------------------------------------------------------------------------------------------

class WndDocument : public CDocument
{
public:

	WndDocument() {}

	~WndDocument() override {}

public:

	Wnd::EDocType GetDocType();

	WndView* GetView();

public:

	void OnCloseDocument() override;

	BOOL OnNewDocument() override;

	BOOL OnOpenDocument(LPCTSTR lpszPathName) override;

	BOOL OnSaveDocument(LPCTSTR lpszPathName) override;

	void Serialize(CArchive& ar) override;

#ifdef _DEBUG
	void AssertValid() const override;

	void Dump(CDumpContext& dc) const override;
#endif

	DECLARE_DYNCREATE(WndDocument)
	DECLARE_MESSAGE_MAP()
};
