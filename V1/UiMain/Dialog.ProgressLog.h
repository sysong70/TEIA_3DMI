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

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

		BOOL DestroyWindow() override;

	private:

		void SetRange(Json::Object& data);

		void SetPosition(Json::Object& data);

		void SetMessage(Json::Object& data);

		void AddLog(Json::Object& data);
		// Last log only
		void SetLogStatus(Json::Object& data);

		void ClearLog();

	private: // Body controls

		CBCGPStatic m_wndMessage;
		CBCGPProgressCtrl m_wndProgress;
		CBCGPListBox m_wndLog;
	};
}
