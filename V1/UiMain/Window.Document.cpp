#include "stdafx.h"
#include "Window.Document.h"
#include "Window.View.h"
#include <File.h>
#include <Path.h>
#include <WStr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



bool Window::IsAllowedFile(const wchar_t* pFilePath)
{
	return IsAllowed3d(pFilePath) || IsAllowed2d(pFilePath);
}



bool Window::IsAllowed3d(const wchar_t* pFilePath)
{
	//:REF - https://docs.techsoft3d.com/exchange/latest/start/supported-formats.html

	//:WARNING - check formats

	const CString EXTENSIONS[] = {
		L"3MF", // 3MF
		L"SAT", L"SAB", // ACIS
		L"3DS", // Autodesk 3DS
		L"IPT", L"IAM", // Autodesk Inventor
		L"NWD", // Autodesk Navisworks
		L"MODEL", L"SESSION", L"DLV", L"EXP", // Catia V4
		L"CATPART", L"CATPRODUCT", L"CATSHAPE", L"CGR", // Catia V5
		L"3DXML", // Catia V6 / 3DExperience
		L"DAE", // COLLADA
		L"ASM", L"NEU", L"PRT", L"XAS", L"XPR", // Creo - Pro/E
		L"FBX", // FBX
		L"GLTF", L"GLB", // GL Transmission Format
		L"MF1", L"ARC", L"UNV", L"PKG", // I-deas
		L"IFC", L"IFCZIP", // IFC
		L"IGS", L"IGES", // IGES
		L"JT", // JT
		L"DGN", // Microstation DGN
		L"PRT", // NX - Unigraphics
		L"X_B", L"X_T", L"XMT", L"XMT_TXT", // Parasolid
		L"PDF", // PDF
		L"PRC", // PRC
		L"HSF", // HSF
		L"RVT, RFA", // Revit
		L"3DM", // Rhino3D
		L"ASM", L"PAR", L"PWD", L"PSM", // Solid Edge
		L"SLDASM", L"SLDPRT", // SolidWorks
		L"STP", L"STEP", L"STPZ", // STEP
		L"STPX", L"STPXZ", // STEP/XML
		L"STL", // Stereo Lithography (STL)
		L"U3D", // U3D
		L"VDA", // VDA-FS
		L"WRL", L"VRML", // VRML
		L"OBJ", // Wavefront OBJ
		L"PTS", L"PTX", L"XYZ", // Point Cloud
	};

	CString ext = Path::GetExtension(pFilePath);
	ext.MakeUpper();

	for (auto pre : EXTENSIONS) {
		if (pre == ext) {
			return true;
		}
	}

	//:WARNING - Creo - Pro/E (case *.1)
	if (WStr::IsDigit(ext)) {
		return true;
	}

	return false;
}



bool Window::IsAllowed2d(const wchar_t* pFilePath)
{
	const CString EXTENSIONS[] = {
		L"DWG", L"DXF" // AutoCAD
		//L"DWF", L"DWFX", // Autodesk DWF
		//L"CATDRAWING",
	};

	CString ext = Path::GetExtension(pFilePath);
	ext.MakeUpper();

	for (auto pre : EXTENSIONS) {
		if (pre == ext) {
			return true;
		}
	}

	return false;
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
