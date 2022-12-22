#include "stdafx.h"
#include "Window.Document.h"
#include "Window.View.h"
#include "Path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



bool Window::IsAllowedFile(const wchar_t* pFilePath)
{
	CString ext = Path::GetExtension(pFilePath);
	ext.MakeLower();

	if (ext == L"3dxml") return true;
	if (ext == L"sat" || ext == L"sab") return true;
	if (ext == L"obj") return true;
	if (ext == L"dwg" || ext == L"dxf") return true;
	if (ext == L"model" || ext == L"exp" || ext == L"session" || ext == L"dlv") return true;
	if (ext == L"catpart") return true;
	if (ext == L"catproduct") return true;
	if (ext == L"catshape") return true;
	if (ext == L"cgr") return true;
	if (ext == L"cgm") return true;
	if (ext == L"hsf") return true;
	if (ext == L"mf1" || ext == L"arc" || ext == L"unv" || ext == L"pkg") return true;
	if (ext == L"igs" || ext == L"iges") return true;
	if (ext == L"ifc" || ext == L"ifczip") return true;
	if (ext == L"ipt" || ext == L"iam") return true;
	if (ext == L"jt") return true;
	if (ext == L"x_t" || ext == L"x_b" || ext == L"xmt" || ext == L"xmt_txt") return true;
	if (ext == L"pdf") return true;
	if (ext == L"ply") return true;
	if (ext == L"pts" || ext == L"ptx" || ext == L"xyz") return true;
	if (ext == L"prc") return true;
	if (ext == L"asm" || ext == L"neu" || ext == L"prt" || ext == L"xas" || ext == L"xpr") return true;
	if (ext == L"rvt" || ext == L"rfa") return true;
	if (ext == L"3dm") return true;
	if (ext == L"asm" || ext == L"par" || ext == L"pwd" || ext == L"psm") return true;
	if (ext == L"sldasm" || ext == L"sldprt") return true;
	if (ext == L"step" || ext == L"stp" || ext == L"stpz" || ext == L"z") return true;
	if (ext == L"stl") return true;
	if (ext == L"u3d") return true;
	if (ext == L"prt") return true;
	if (ext == L"vda") return true;
	if (ext == L"wrl" || ext == L"vrml") return true;

	return false;
}



CString Window::GetDocTypeName(DocType e)
{
	switch (e) {
	case DocType::ACIS:        return L"ACIS";
	case DocType::CATIA4:      return L"CATIA4";
	case DocType::CATIA5:      return L"CATIA5";
	case DocType::CGR:         return L"CGR";
	case DocType::IDEAS:       return L"IDEAS";
	case DocType::IFC:         return L"IFC";
	case DocType::IGES:        return L"IGES";
	case DocType::Inventor:    return L"Inventor";
	case DocType::JT:          return L"JT";
	case DocType::Parasolid:   return L"Parasolid";
	case DocType::PDF:         return L"PDF";
	case DocType::PRC:         return L"PRC";
	case DocType::ProEngineer: return L"ProEngineer";
	case DocType::Rhino:       return L"Rhino";
	case DocType::SolidEdge:   return L"SolidEdge";
	case DocType::Solidworks:  return L"Solidworks";
	case DocType::STEP:        return L"STEP";
	case DocType::STL:         return L"STL";
	case DocType::Universal3D: return L"Universal3D";
	case DocType::Unigraphics: return L"Unigraphics";
	case DocType::VDAFS:       return L"VDAFS";
	case DocType::VRML:        return L"VRML";
	case DocType::DWG3D:       return L"DWG3D";
	case DocType::DWG2D:       return L"DWG2D";
	case DocType::DXF:         return L"DXF";
	case DocType::Revit:       return L"Revit";
	case DocType::HPGL:        return L"HPGL";

	default:
		DEBUG_STOP;
		return L"Unknown";
	}
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



int Window::Document::GetId()
{
	return GetView()->GetId();
}



Window::View* Window::Document::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos != nullptr) {
		View* pView = DYNAMIC_DOWNCAST(View,  GetNextView(pos));
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
	GetView()->SetFilePath(lpszPathName);

	return TRUE;
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
