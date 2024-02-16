#include "stdafx.h"
#include "Window.Document.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Window;

IMPLEMENT_DYNCREATE(Document, CDocument)

BEGIN_MESSAGE_MAP(Document, CDocument)
END_MESSAGE_MAP()



Window::Document::Document()
{
}



Window::Document::~Document()
{
}



Window::Document::EType Window::Document::GetCateogry()
{
	CString filePath = GetPathName();

	//:WARNING - new document
	if (filePath.IsEmpty()) {
		return EType::Doc3d;
	}

	CString ext = Path::GetExtension(filePath);
	ext.MakeLower();

	if (ext == "dwg" || ext == "dxf") {
		return EType::Doc2d;
	}
	else {
		return EType::Doc3d;
	}
}



Window::View* Window::Document::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos != nullptr) {
		//View* pView = DYNAMIC_DOWNCAST(View, GetNextView(pos));
		View* pView = (View*)GetNextView(pos);
		if (pView != nullptr) {
			return pView;
		}
	}

	RETURN_NULL;
}



void Window::Document::OnCloseDocument()
{
	__super::OnCloseDocument();
}



BOOL Window::Document::OnNewDocument()
{
	if (__super::OnNewDocument() == FALSE) {
		RETURN_FALSE;
	}

	return TRUE;
}



BOOL Window::Document::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (File::IsExist(lpszPathName)) {
		return TRUE;
	}
	else {
		BCGPMessageBox(L"File not found!");
		return FALSE;
	}
}



BOOL Window::Document::OnSaveDocument(LPCTSTR lpszPathName)
{
	return 0;
}



void Window::Document::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	}
	else {
		// TODO: add loading code here
	}
}

#ifdef _DEBUG

void Window::Document::AssertValid() const
{
	__super::AssertValid();
}



void Window::Document::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

#undef PRESET
