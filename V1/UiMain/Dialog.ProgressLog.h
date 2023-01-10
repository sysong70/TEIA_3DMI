#pragma once

#include "Dialog.Standard.h"



namespace Dialog
{
	class ProgressLog : public Standard
	{
		DECLARE_DYNAMIC(ProgressLog)

	public:

		ProgressLog(CWnd* pParent = nullptr);

		~ProgressLog() override;

		Signal::Target GetSignalTargetId() override;

		void ReceiveSignal(Json::Object* pData) override;

	protected:

		void DoDataExchange(CDataExchange* pDX) override;

		BOOL OnInitDialog() override;

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

		BOOL DestroyWindow() override;

	private:

		void StartMarquee(bool start = true);

		void SetMessage(Json::Object& data);

		void AddLog(Json::Object& data);

		void SetLogStatus(Json::Object& data);

	private: // Body controls

		CBCGPCircularProgressIndicatorCtrl m_wndIndicator;
		CBCGPStatic m_wndMessage;
		CBCGPListBox m_wndLog;
	};
}
