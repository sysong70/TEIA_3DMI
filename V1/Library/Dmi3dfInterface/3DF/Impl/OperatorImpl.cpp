#include "StdAfx.h"

#include "OperatorImpl.h"

#include "../Window.h"

using namespace H3DF;

H3DF::Operator::OperatorImpl::OperatorImpl()
{
	m_eType = H3DF::Type::HighlightControl;
}

H3DF::Operator::OperatorImpl::OperatorImpl(WindowKey const & cInWindow)
{
	m_eType = H3DF::Type::HighlightControl;

	m_pcWindow = (WindowKey *)&cInWindow;
}

BaseView * H3DF::Operator::OperatorImpl::GetBaseView()
{
	return (BaseView *)m_pcWindow->GetBaseView();
}

BaseView * H3DF::Operator::OperatorImpl::GetBaseView() const
{
	return (BaseView *)m_pcWindow->GetBaseView();
}
