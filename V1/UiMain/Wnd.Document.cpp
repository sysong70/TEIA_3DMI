#include "stdafx.h"

#include "Wnd.Document.h"
#include "Wnd.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

IMPLEMENT_DYNCREATE(WndDocument, CDocument)

BEGIN_MESSAGE_MAP(WndDocument, CDocument)
END_MESSAGE_MAP()



Wnd::EDocType WndDocument::GetDocType()
{
	CString filePath = GetPathName();

	// WARNING - new document
	if (filePath.IsEmpty()) {
		return Wnd::EDocType::Model;
	}

	CString ext = Path::GetExtension(filePath);
	ext.MakeLower();

	if (ext == "dwg" || ext == "dxf") {
		return Wnd::EDocType::Drawing;
	}
	else {
		return Wnd::EDocType::Model;
	}
}



WndView* WndDocument::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos != nullptr) {
		//WndView* pView = DYNAMIC_DOWNCAST(WndView, GetNextView(pos));
		WndView* pView = (WndView*)GetNextView(pos);
		if (pView != nullptr) {
			return pView;
		}
	}

	RETURN_NULL;
}



void WndDocument::OnCloseDocument()
{
	__super::OnCloseDocument();
}



BOOL WndDocument::OnNewDocument()
{
	if (__super::OnNewDocument() == FALSE) {
		RETURN_FALSE;
	}

	return TRUE;
}



BOOL WndDocument::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (File::IsExist(lpszPathName)) {
		return TRUE;
	}
	else {
		BCGPMessageBox(L"File not found!");
		return FALSE;
	}
}



BOOL WndDocument::OnSaveDocument(LPCTSTR lpszPathName)
{
	return 0;
}



void WndDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	}
	else {
		// TODO: add loading code here
	}
}

#ifdef _DEBUG

void WndDocument::AssertValid() const
{
	__super::AssertValid();
}



void WndDocument::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG
