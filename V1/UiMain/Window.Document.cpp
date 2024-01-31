#include "stdafx.h"
#include "Window.Document.h"
#include "Window.View.h"
#include "Facility.AppResources.h"
#include <File.h>
#include <Path.h>
#include <WStr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define PRESET WindowPreset

namespace WindowPreset
{
	using Extensions = std::vector<CString>;

	Extensions Extensions3d;
	Extensions Extensions2d;

	Extensions& Get3dExtensions() {
		// generate extensions
		if (Extensions3d.size() == 0) {
			Json::Array& items = TheAppResources
				.GetDialog("FileOptions")
				.GetArray("tree")
				.GetObject(0)
				.GetArray("items");

			for (auto item : items.GetBuffer()) {
				Json::Object& target = item->AsObject();
				if (target.GetBoolean("visible", true) == false) {
					continue;
				}

				Json::Array& extensions = target.GetArray("ext");
				for (auto ext : extensions.GetBuffer()) {
					Extensions3d.push_back(ext->AsString());
				}
			}
		}

		return Extensions3d;
	}

	Extensions& Get2dExtensions() {
		//:TODO
		if (Extensions2d.size() == 0) {
			Extensions2d.push_back(L"DWG");
			Extensions2d.push_back(L"DXF");
		}

		return Extensions2d;
	}
}



bool Window::IsAllowedFile(const wchar_t* pFilePath)
{
	return IsAllowed3d(pFilePath) || IsAllowed2d(pFilePath);
}

//:REF - https://docs.techsoft3d.com/exchange/latest/start/supported-formats.html

bool Window::IsAllowed3d(const wchar_t* pFilePath)
{
	CString extension = Path::GetExtension(pFilePath);
	extension.MakeUpper();

	//:WARNING - Creo/ProE (case *.1)
	if (WStr::IsDigit(extension)) {
		return true;
	}

	for (auto& item : PRESET::Get3dExtensions()) {
		if (item == extension) {
			return true;
		}
	}

	return false;
}



bool Window::IsAllowed2d(const wchar_t* pFilePath)
{
	CString extension = Path::GetExtension(pFilePath);
	extension.MakeUpper();

	for (auto& item : PRESET::Get2dExtensions()) {
		if (item == extension) {
			return true;
		}
	}

	return true;
}



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
