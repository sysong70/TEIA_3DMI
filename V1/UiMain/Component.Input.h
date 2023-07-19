#pragma once

#include "Component.h"
#include "Window.h"
#include <Signal.h>
#include <vector>



namespace Component
{
	class Input
	{
	public:

		Input();

		~Input();

		bool SetView(Window::View* pView);

		void SetMode(Signal::EInputMode eMode);

		void SetValue(double value);

		void SetValue(int value);

		void SetValue(double x, double y);

		void SetValue(double x, double y, double z);

	public:

		void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	protected:

		void Initialize();

		Window::View* m_pView = nullptr;
		Signal::EInputMode m_eMode = Signal::EInputMode::Unknown; //:CHECK
		bool m_changed = false;

		CString m_texts[4];
		CString& m_text = m_texts[0];
		int m_textIndex = 0;
		int m_selectIndex = -1; // text index or -1
		int m_cursorPos = 0;

		bool IsValid();

		bool OnCharReal(UINT nChar, UINT nRepCnt, UINT nFlags);

		bool OnCharInteger(UINT nChar, UINT nRepCnt, UINT nFlags);

		bool OnCharPoint2d(UINT nChar, UINT nRepCnt, UINT nFlags);

		bool OnCharPoint3d(UINT nChar, UINT nRepCnt, UINT nFlags);

		void SendDataToRenderer(bool forced = false);
	};
}

