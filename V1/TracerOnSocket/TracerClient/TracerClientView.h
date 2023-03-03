#pragma once



class CTracerClientView : public CView
{
protected:

	CTracerClientView() noexcept;

	DECLARE_DYNCREATE(CTracerClientView)

public:

	CTracerClientDoc* GetDocument() const;

public:

	virtual void OnDraw(CDC* pDC);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual ~CTracerClientView();

protected:

	DECLARE_MESSAGE_MAP()
};
