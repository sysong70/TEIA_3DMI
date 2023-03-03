#include "pch.h"
#include "framework.h"
#include "TracerClientApp.h"
#include "TracerClientDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTracerClientDoc

IMPLEMENT_DYNCREATE(CTracerClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CTracerClientDoc, CDocument)
END_MESSAGE_MAP()


// CTracerClientDoc 생성/소멸

CTracerClientDoc::CTracerClientDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CTracerClientDoc::~CTracerClientDoc()
{
}

BOOL CTracerClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CTracerClientDoc serialization

void CTracerClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


#ifdef _DEBUG
void CTracerClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTracerClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTracerClientDoc 명령
