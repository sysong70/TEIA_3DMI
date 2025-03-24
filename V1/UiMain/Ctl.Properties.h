#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

namespace Prop
{
	class DurationCtrl;
	class IconComboBoxCtrl;
	class SliderCtrl;

	class Color;
	class Coordinate;
	class ComboButton;
	class CommandButton;
	class CustomColor;
	class CustomDialog;
	class CustomState;
	class Duration;
	class FoldersDialog;
	class FontCombo;
	class HexValue;
	class IconCombo;
	class IconList;
	class Password;
	class RangeValidation;
	class Slider;
	class TwoButtons;

#pragma region Custom Controls

	class DurationCtrl : public CBCGPDurationCtrl
	{
		afx_msg void OnKillFocus(CWnd* pNewWnd);

		DECLARE_MESSAGE_MAP()
	};

//--------------------------------------------------------------------------------------------------

	class IconComboBoxCtrl : public CBCGPComboBox
	{
	public:

		CBCGPToolBarImages& Icons;
		CStringList& IconNames;

		IconComboBoxCtrl(CBCGPToolBarImages& icons, CStringList& iconNames);

		virtual ~IconComboBoxCtrl();

	public:

		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

		afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

		DECLARE_MESSAGE_MAP()
	};

//--------------------------------------------------------------------------------------------------

	class SliderCtrl : public CBCGPSliderCtrl
	{
	public:

		Slider*	PropPtr = nullptr;
		COLORREF BkColor;
		CBrush BkBrush;

		SliderCtrl(Slider* pProp, COLORREF clrBack);

		~SliderCtrl() override;

	public:

		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

		afx_msg void HScroll(UINT nSBCode, UINT nPos);

		DECLARE_MESSAGE_MAP()
	};

#pragma endregion // REGION

#pragma region Propertis

	class Color : public CBCGPColorProp
	{
	public:

		Color::Color(const CString& name, const COLORREF& color, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = NULL);

	public:

		BOOL IsDroppedDown() const override;

		void OnCloseCombo() override;
	};

//--------------------------------------------------------------------------------------------------

	class Coordinate : public CBCGPProp
	{
	public:

		// Combo
		CBCGPToolBarImages Icons;
		CStringList IconNames;
		// Buttons
		CBCGPToolBarImages ButtonImages;
		int ClickedButton = -1;

		Coordinate(const CString& name, const CString& value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		void AdjustButtonRect() override;

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDPIChanged(UINT nDPIOld, UINT nDPINew) override;

		void OnDrawButton(CDC* pDC, CRect rectButton) override;

		BOOL OnUpdateValue() override;

	public:

		void LoadImages();
	};

//--------------------------------------------------------------------------------------------------

	class ComboButton : public CBCGPProp
	{
	public:

		ComboButton(const CString& name, const _variant_t& value);

	public:

		void AdjustButtonRect() override;

		//LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDrawButton(CDC* pDC, CRect rectButton) override;
	};

//--------------------------------------------------------------------------------------------------

	class CommandButton : public CBCGPProp
	{
	public:

		CString Title;
		bool Highlighted = false;

		CommandButton(const CString& name, const CString& title, LPCTSTR lpDescr, DWORD_PTR data = NULL);

	public:

		BOOL OnClickValue(UINT uiMsg, CPoint point) override;
		
		void OnDrawValue(CDC* pDC, CRect rect) override;

		BOOL OnEdit(LPPOINT lptClick) override;

		void OnLeaveMouse() override;

		BOOL OnSetCursor() const override;
	};

//--------------------------------------------------------------------------------------------------

	class CustomColor : public CBCGPColorProp
	{
	public:

		CustomColor(const CString& name, const COLORREF& color, LPCTSTR lpDescr = NULL);

	public:

		//LPCTSTR GetXMLTagName() const override;

		BOOL OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL bIsAutomatic, BOOL bIsSelected, BOOL bOnPopupMenu) override;
	};

//--------------------------------------------------------------------------------------------------

	class CustomDescription : public CBCGPProp
	{
	public:

		CustomDescription(const CString& name, const _variant_t& value, LPCTSTR lpDescr = NULL);

	public:

		//virtual LPCTSTR GetXMLTagName() const;

		virtual void OnDrawDescription(CDC* pDC, CRect rect);
	};

//--------------------------------------------------------------------------------------------------

	class CustomDialog : public CBCGPProp
	{
	public:

		CustomDialog(const CString& name, const _variant_t& value);

	public:

		//LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;
	};

//--------------------------------------------------------------------------------------------------

	class CustomState : public CBCGPProp
	{
	public:

		int AnimationStep = 0;
		bool Stopped = false;
		CBCGPToolBarImages Images;

		CustomState(const CString& name, LPCTSTR lpValue, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

		void StepAnimation();

	public:

		//LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDrawStateIndicator(CDC* pDC, CRect rect) override;
	};

//--------------------------------------------------------------------------------------------------

	class Duration : public CBCGPProp
	{
	public:

		DurationCtrl DurationWnd;

		Duration(const CString& name, const COleDateTimeSpan& duration, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

		~Duration() override;

		COleDateTimeSpan GetDuration() const;

		void SetDuration(COleDateTimeSpan duration);

	public:

		void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		BOOL DoPaste() override;

		//LPCTSTR GetXMLTagName() const override;

		void OnDrawValue(CDC* pDC, CRect rect) override;

		void OnKillSelection(CBCGPProp* pNewSel) override;

		void OnPosSizeChanged(CRect rectOld) override;

		void OnSetSelection(CBCGPProp* pOldSel) override;

		BOOL OnUpdateValue() override;

		BOOL PushChar(UINT nChar) override;

		void SetValue(const _variant_t& varValue) override;
	};

//--------------------------------------------------------------------------------------------------

	class FoldersDialog : public CBCGPProp
	{
	public:

		FoldersDialog(const CString& name, const CString value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;
	};

//--------------------------------------------------------------------------------------------------

	class FontCombo : public CBCGPProp
	{
	public:

		FontCombo(const CString& name, const CString value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;
	};

//--------------------------------------------------------------------------------------------------

	class HexValue : public CBCGPProp
	{
	public:

		HexValue(const CString& name, long lValue, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		CString FormatProperty() override;

		//LPCTSTR GetXMLTagName() const override;

		BOOL ParseValue(const CString& str) override;

		BOOL SerializeValue(CString& str) override;

		BOOL TextToVar(const CString& strText) override;
	};

//--------------------------------------------------------------------------------------------------

	class IconCombo : public CBCGPProp
	{
	public:

		IconCombo(const CString& name, const CString& value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL, CBCGPToolBarImages* pImageList = NULL);

		bool AddOption(LPCTSTR lpOption, int nIcon = -1, int nIndent = 0);

	public:

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		//LPCTSTR GetXMLTagName() const override;

		BOOL OnEdit(LPPOINT lpClick) override;

	public:

		CImageList m_imageList;
		std::vector<int> Icons;
		std::vector<int> m_indents;
	};

//--------------------------------------------------------------------------------------------------

	class IconList : public CBCGPProp
	{
	public:

		CBCGPToolBarImages& Icons;
		CStringList IconNames;

		IconList(const CString& name, CBCGPToolBarImages& icons, int nSelected, CStringList* pNames = NULL, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		CComboBox* CreateCombo(CWnd* pWndParent, CRect rect) override;

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		//LPCTSTR GetXMLTagName() const override;

		void OnDrawValue(CDC* pDC, CRect rect) override;
	};

//--------------------------------------------------------------------------------------------------

	class Password : public CBCGPProp
	{
		static const TCHAR DefaultChar = L'*';

	public:

		Password(const CString& name, const CString& password, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

	public:

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		CString FormatProperty() override;

		//LPCTSTR GetXMLTagName() const override;
		// WARNING - Don't save password in XML!
		CString GetXMLValue() const override;
		// WARNING - Don't copy password to the clipboard!
		BOOL IsCopyAvailable() const override;
	};

//--------------------------------------------------------------------------------------------------

	class RangeValidation : public CBCGPProp
	{
	public:

		long MinValue = LONG_MIN;
		long MaxValue = LONG_MAX;

		RangeValidation(const CString& name, long value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

		void SetRange(long minValue, long maxValue);

	public:

		//LPCTSTR GetXMLTagName() const override;

		BOOL OnEndEdit() override;

		BOOL OnUpdateValue() override;
	};

//--------------------------------------------------------------------------------------------------

	class Slider : public CBCGPProp
	{
	public:

		int MinValue = INT_MIN;
		int MaxValue = INT_MAX;
		int Step = 1;

		Slider(const CString& name, long value, LPCTSTR lpDescr = NULL, DWORD_PTR data = NULL);

		void SetRange(int minValue, int maxValue, int step);

	public:

		CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat) override;

		//LPCTSTR GetXMLTagName() const override;

		BOOL OnClickValue(UINT uiMsg, CPoint point) override;

		BOOL OnSetCursor() const override;

		BOOL OnUpdateValue() override;
	};

//--------------------------------------------------------------------------------------------------

	class TwoButtons : public CBCGPProp
	{
	public:

		CBCGPToolBarImages Images;
		int ClickedButton = 0;

		TwoButtons(const CString& name, const _variant_t& value);

	public:

		void AdjustButtonRect() override;

		//LPCTSTR GetXMLTagName() const override;

		BOOL HasButton() const override;

		void OnClickButton(CPoint point) override;

		void OnDPIChanged(UINT nDPIOld, UINT nDPINew) override;

		void OnDrawButton(CDC* pDC, CRect rectButton) override;

	public:

		void LoadImages();
	};

#pragma endregion // REGION
}
