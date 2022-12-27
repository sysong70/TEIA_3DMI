#pragma once

#include "Dialog.h"
#include "resource.h"
#include "Facility.Base.h"
#include "Json.h"
#include "Signal.h"



namespace Dialog
{
	class Base : public CBCGPDialog
	{
	public:

		Base(UINT nIDTemplate, CWnd* pParent = nullptr);

		~Base() override;
		// Create and show window from dialog pointer
		void DoModaless();

	public:

		virtual Signal::Target GetSignalTargetId();
		// return dialog result
		virtual Facility::Base* GetResultData(bool bNewInstance = false);
		// delete after using this data
		virtual Json::Object* GetResult();

		virtual void ReceiveSignal(Json::Object* pData);

	protected:

		virtual CRect ConstructHeader(const CRect& boundary);
	
		virtual CRect ConstructBody(const CRect& boundary);
		
		virtual CRect ConstructFooter(const CRect& boundary);
		// exclude margin
		virtual CRect GetBodyRect();
		// include margin
		virtual CSize GetBodySize();

	protected: // Windows message handler

		BOOL OnInitDialog() override;

		void PostNcDestroy() override;

		afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

		DECLARE_MESSAGE_MAP()

	protected: // Window helper

		// call after adding child controls
		CSize AdjustWindowSize(CSize clientSize);

		void EnableParent(bool enable = true);

		CRect GetClientArea();

		CFont* GetDefaultFont();
		// gap between client area and window nc area
		CRect GetPadding();
		// gap bwtween bottom right
		CSize GetFrameThickness();
		// dialog control size 32x32 in resource
		CSize GetScaled32();
		// Min, Max dialog size limit
		void SetSizeLimit(bool bMinLimit, bool bMaxLimit);

		void SetSizeLimit(POINT min, POINT max);

	protected:

		UINT m_nTemplateId = IDD_DMI_STANDARD; // dialog tamplate id
		CSize m_windowSize;
		int m_nHeaderHeight = 0;
		int m_nFooterHeight = 0;
		MINMAXINFO m_sizeLimit = {}; // For OnGetMinMaxInfo
	};
}
