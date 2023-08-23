#include "StdAfx.h"
#include "3DF.Operator.KeyboardTest.h"
#include "3DF.Painter.h"
#include "3DF.Window.h"
#include "3DF.BaseView.h"
#include "../Signal/Signal.h"

USING_3DF_NAMESPACE

Operator::KeyboardTest::KeyboardTest(WindowKey* pWindow)
{
	m_pWindow = pWindow;
}



void Operator::KeyboardTest::OnKeyboard(Json::Object& input)
{
	using namespace H3DF::Painter;

	m_text = input.GetString(SKW_VALUE);
	m_row = input.GetInteger(SKW_ROW);
	m_column = input.GetInteger(SKW_COLUMN);

	HC_Open_Segment_By_Key(m_pWindow->GetBaseView()->GetConstructionKey());
	{
		Segment::SetVisibility("string cursors", true);
		HC_Delete_By_Key(m_textKey);
		HC_Delete_By_Key(m_cursorKey);

		m_textKey = Text::Create(H3DF::Point(), m_text);
		m_cursorKey = Cursor::Create(m_textKey, m_row, m_column);
	}
	HC_Close_Segment();

	m_pWindow->GetBaseView()->Update();
}
