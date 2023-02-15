#pragma once

#include "Window.h"



namespace Window
{
	bool IsAllowedFile(const wchar_t* pFilePath);

	bool IsAllowed3d(const wchar_t* pFilePath);

	bool IsAllowed2d(const wchar_t* pFilePath);



	class Document : public CDocument
	{
	protected:

		Document();

		DECLARE_DYNCREATE(Document)

	public:

		enum EType
		{
			Unknown = -1,
			Doc3d,
			Doc2d,
		};

		~Document() override;

		EType GetCateogry();

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
