#pragma once

#include "Component.h"



namespace Property
{
	class Slider;

#pragma region Custom Controls

	class DurationCtrl : public CBCGPDurationCtrl
	{
		afx_msg void OnKillFocus(CWnd* pNewWnd);

		DECLARE_MESSAGE_MAP()
	};



	class IconComboBox : public CBCGPComboBox
	{
	public:

		IconComboBox(CBCGPToolBarImages& icons, CStringList& iconNames);

		virtual ~IconComboBox();

	protected:

		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

		afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

		DECLARE_MESSAGE_MAP()

	protected:

		CBCGPToolBarImages& m_icons;
		CStringList& m_iconNames;
	};



	class SliderCtrl : public CBCGPSliderCtrl
	{
	public:

		SliderCtrl(Slider* pProp, COLORREF clrBack);

		~SliderCtrl() override;

	protected:

		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

		afx_msg void HScroll(UINT nSBCode, UINT nPos);

		DECLARE_MESSAGE_MAP()

	protected:

		Slider* m_pProp;
		COLORREF m_clrBack;
		CBrush m_brBackground;
	};

#pragma endregion //:REGION

	class ComboBoxEx : public CBCGPProp
	{
	public:

		ComboBoxEx(const CString& name, const CString& value, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0, CBCGPToolBarImages* pImageList = NULL);

		bool AddOption(LPCTSTR lpOption, int nIcon = -1, int nIndent = 0);

	protected:

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		LPCTSTR GetXMLTagName() const override;

		BOOL OnEdit(LPPOINT lpClick) override;

	protected:

		CImageList m_imageList;
		std::vector<int> m_icons;
		std::vector<int> m_indents;
	};



	class ComboButton : public CBCGPProp
	{
	public:

		ComboButton(const CString& name, const _variant_t& value);

	protected:

		void AdjustButtonRect() override;

		LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDrawButton(CDC* pDC, CRect rectButton) override;
	};



	class CustomColor : public CBCGPColorProp
	{
	public:

		CustomColor(const CString& name, const COLORREF& color, LPCTSTR lpDescr = NULL);

	protected:

		LPCTSTR GetXMLTagName() const override;

		BOOL OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL bIsAutomatic, BOOL bIsSelected, BOOL bOnPopupMenu) override;
	};



	class CustomDescription : public CBCGPProp
	{
	public:

		CustomDescription(const CString& name, const _variant_t& value, LPCTSTR lpDescr = NULL);

	protected:

		virtual LPCTSTR GetXMLTagName() const;

		virtual void OnDrawDescription(CDC* pDC, CRect rect);
	};



	class CustomDialog : public CBCGPProp
	{
	public:

		CustomDialog(const CString& name, const _variant_t& value);

	protected:

		LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;
	};



	class CustomState : public CBCGPProp
	{
	public:

		CustomState(const CString& name, LPCTSTR lpValue, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

		void AnimationStep();

	protected:

		LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDrawStateIndicator(CDC* pDC, CRect rect) override;

	protected:

		int m_nAnimationStep = 0;
		bool m_bStopped = false;
		CBCGPToolBarImages m_images;
	};



	class Duration : public CBCGPProp
	{
	public:

		Duration(const CString& name, const COleDateTimeSpan& duration, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

		~Duration() override;

		COleDateTimeSpan GetDuration() const;

		void SetDuration(COleDateTimeSpan duration);

	public:

		void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		BOOL DoPaste() override;

		LPCTSTR GetXMLTagName() const override;

		void OnDrawValue(CDC* pDC, CRect rect) override;

		void OnKillSelection(CBCGPProp* pNewSel) override;

		void OnPosSizeChanged(CRect rectOld) override;

		void OnSetSelection(CBCGPProp* pOldSel) override;

		BOOL OnUpdateValue() override;

		BOOL PushChar(UINT nChar) override;

		void SetValue(const _variant_t& varValue) override;

	protected:

		DurationCtrl m_wndDuration;
	};



	class HexValue : public CBCGPProp
	{
	public:

		HexValue(const CString& name, long lValue, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

	protected:

		CString FormatProperty() override;

		LPCTSTR GetXMLTagName() const override;

		BOOL ParseValue(const CString& str) override;

		BOOL SerializeValue(CString& str) override;

		BOOL TextToVar(const CString& strText) override;
	};



	class IconList : public CBCGPProp
	{
	public:

		IconList(const CString& name, CBCGPToolBarImages& icons, int nSelected, CStringList* pNames = NULL, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

	protected:

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		LPCTSTR GetXMLTagName() const override;

		void OnDrawValue(CDC* pDC, CRect rect) override;

	protected:

		CBCGPToolBarImages& m_icons;
		CStringList m_iconNames;
	};



	class Password : public CBCGPProp
	{
		static const TCHAR DefaultChar = L'*';

	public:

		Password(const CString& name, const CString& password, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

	protected:

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		CString FormatProperty() override;

		LPCTSTR GetXMLTagName() const override;
		//:WARNING - Don't save password in XML!
		CString GetXMLValue() const override;
		//:WARNING - Don't copy password to the clipboard!
		BOOL IsCopyAvailable() const override;
	};



	class RangeValidation : public CBCGPProp
	{
	public:

		RangeValidation(const CString& name, long value, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

		void SetRange(long minValue, long maxValue);

	protected:

		LPCTSTR GetXMLTagName() const override;

		BOOL OnEndEdit() override;

		BOOL OnUpdateValue() override;

	protected:

		long m_minValue = LONG_MIN;
		long m_maxValue = LONG_MAX;
	};



	class Slider : public CBCGPProp
	{
	public:

		friend class SliderCtrl;

		Slider(const CString& name, long value, LPCTSTR lpDescr = NULL, DWORD_PTR data = 0);

	protected:

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		LPCTSTR GetXMLTagName() const override;

		BOOL OnClickValue(UINT uiMsg, CPoint point) override;

		BOOL OnSetCursor() const override;

		BOOL OnUpdateValue() override;
	};



	class TwoButtons : public CBCGPProp
	{
	public:

		TwoButtons(const CString& name, const _variant_t& value);

	protected:

		void AdjustButtonRect() override;

		LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDPIChanged(UINT nDPIOld, UINT nDPINew) override;

		void OnDrawButton(CDC* pDC, CRect rectButton) override;

	protected:

		CBCGPToolBarImages m_images;
		int m_nClickedButton = 0;

		void LoadImages();
	};
}
