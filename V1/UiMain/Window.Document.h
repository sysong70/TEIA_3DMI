#pragma once

#include "Window.h"



namespace Window
{
	enum class EDocType
	{
		Unknown = -1,

		ACIS,
		CATIA4,
		CATIA5,
		CGR,
		IDEAS,
		IFC,
		IGES,
		Inventor,
		JT,
		Parasolid,
		PDF,
		PRC,
		ProEngineer,
		Rhino,
		SolidEdge,
		Solidworks,
		STEP,
		STL,
		Universal3D,
		Unigraphics,
		VDAFS,
		VRML,
		DWG3D,
		DWG2D,
		DXF,
		Revit,
		HPGL,
	};

	bool IsAllowedFile(const wchar_t* pFilePath);

	CString GetDocTypeName(EDocType e);



	class Document : public CDocument
	{
	protected:

		Document();

		DECLARE_DYNCREATE(Document)

	public:

		~Document() override;

		CString GetFilePath();

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

	protected:

		CString m_sFilePath;
	};
}
