#include "StdAfx.h"

#include "OperatorImpl.h"

#include "../Kernel.DocView.h"
#include "Kernel.DocViewImpl.h"

#include <3DF/Window.h>
#include <Sprocket/3DF.View.h>

using namespace KERNEL;

//== Visual Effects class ==========================================================================

KERNEL::Operator::OperatorImpl::OperatorImpl(const KERNEL::DocView * pcInDocView)
{
	DEBUG_VALID(pcInDocView);

	m_eType = KERNEL::Type::Operator;
	m_pcDocView = pcInDocView;
}

H3DF::WindowKey & KERNEL::Operator::OperatorImpl::Window()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

const H3DF::WindowKey & KERNEL::Operator::OperatorImpl::Window() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

H3DF::View & KERNEL::Operator::OperatorImpl::View()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

const H3DF::View & KERNEL::Operator::OperatorImpl::View() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

void KERNEL::Operator::OperatorImpl::Copy(OperatorImpl * pcInThat)
{
	DEBUG_VALID(pcInThat);

	m_eType = pcInThat->m_eType;
	m_pcDocView = pcInThat->m_pcDocView;
}

Signal::Delivery & KERNEL::Operator::OperatorImpl::Delivery()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

const Signal::Delivery & KERNEL::Operator::OperatorImpl::Delivery() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

DocView & KERNEL::Operator::OperatorImpl::GetDocView()
{
	DEBUG_VALID(m_pcDocView);
	return *((KERNEL::DocView *)m_pcDocView);
}

const DocView & KERNEL::Operator::OperatorImpl::GetDocView() const
{
	DEBUG_VALID(m_pcDocView);
	return *m_pcDocView;
}