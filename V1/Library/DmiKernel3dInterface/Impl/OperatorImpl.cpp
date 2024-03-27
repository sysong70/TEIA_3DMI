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

H3DF::ModelImpl & KERNEL::Operator::OperatorImpl::GetModelImpl()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	H3DF::ModelImpl * pcModelImpl = (H3DF::ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	return *pcModelImpl;
}

const H3DF::ModelImpl & KERNEL::Operator::OperatorImpl::GetModelImpl() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	H3DF::ModelImpl * pcModelImpl = (H3DF::ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	return *pcModelImpl;
}

// Update하기전에 Hightlight된 것들을 모두 Unhighlight하고, SnapItem을 모두 Reset한다.
void KERNEL::Operator::OperatorImpl::PrepareUpdate()
{
	GetDocView().Canvas().GetFrontView().SuppressUpdate(true);

	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(GetDocView().GetImpl());
	pcDocViewImpl->Select().UnhighlightEverything();
	pcDocViewImpl->Select().ResetSnapItems(false);
}

// Update가 완료되면, View를 Update한다.
void KERNEL::Operator::OperatorImpl::Updated()
{
	GetDocView().Canvas().GetFrontView().SuppressUpdate(false);
	GetDocView().Canvas().GetFrontView().Update();
}


