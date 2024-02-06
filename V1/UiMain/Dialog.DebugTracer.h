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

		afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

		void OnContextCommand(UINT id);

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

	public:

		void AddLog(Json::Object& data);
		// local
		void AddLog(const wchar_t* pFormat, ...);

		void AddLog(CString log);

		void ClearLog();

		void SaveLog(Json::Object& data);

		void SaveLog(CString path, bool clear = true);

	private: // Body controls

		CBCGPListBox m_wndLog;
	};
}
