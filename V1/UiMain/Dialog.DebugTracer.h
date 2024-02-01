#pragma once

#include "Dialog.Standard.h"



namespace Dialog
{
	class DebugTracer : public Standard
	{
	protected:

		DECLARE_DYNAMIC(DebugTracer)

		DebugTracer(CWnd* pParent = nullptr);

		~DebugTracer() override;

	public:

		Signal::Target GetSignalTargetId() override;

		void ReceiveSignal(Json::Object* pData) override;

	protected:

		void DoDataExchange(CDataExchange* pDX) override;

		BOOL OnInitDialog() override;

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

	private:

		void AddLog(Json::Object& data);

		void ClearLog();

		void SaveLog(Json::Object& data);

	private: // Body controls

		CBCGPListBox m_wndLog;
	};
}
