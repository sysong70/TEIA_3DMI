#pragma once

class HPSExchangeProgressDialog;


class DmiView : public CView
{
protected: // create from serialization only
	DmiView();
	DECLARE_DYNCREATE(DmiView)

// Attributes
public:
	DmiDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC * pDC);  // overridden to draw this view (삭제하면 않됨)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~DmiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnFilePrintPreview();
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

public:
	void SetFilePathName(CString strFilePathName) { m_strFilePathName = strFilePathName; }

private:
	CString m_strFilePathName;

	bool m_bMessageFlag = true;
	
	bool m_bFileOpenFlag = true;
	int m_nUpdateCount = 0;
	bool m_bFirstPaintFlag = true;
public:
	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnUpdate(CView * /*pSender*/, LPARAM /*lHint*/, CObject * /*pHint*/);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnActivateView(BOOL bActivate, CView * pActivateView, CView * pDeactiveView);
};

#ifndef _DEBUG  // debug version in RtView.cpp
inline DmiDoc* DmiView::GetDocument() const
	{ return reinterpret_cast<DmiDoc*>(m_pDocument); }
#endif

