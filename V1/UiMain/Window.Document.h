#pragma once

#include "Window.h"

//--------------------------------------------------------------------------------------------------

namespace Window
{
	class Document : public CDocument
	{
	protected:

		Document();

		DECLARE_DYNCREATE(Document)

	public:

		~Document() override;

		EDocType GetDocType();

		View* GetView();

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

		DECLARE_MESSAGE_MAP()
	};
}
