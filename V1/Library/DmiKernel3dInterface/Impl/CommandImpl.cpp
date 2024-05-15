#include "StdAfx.h"

#include "CommandImpl.h"

#include "../Kernel.DocView.h"
#include "Kernel.DocViewImpl.h"

#include <3DF/Window.h>
#include <Sprocket/3DF.View.h>

using namespace KERNEL;

//== Visual Effects class ==========================================================================

KERNEL::Command::CommandImpl::CommandImpl(const KERNEL::DocView * pcInDocView)
{
	DEBUG_VALID(pcInDocView);

	m_eType = KERNEL::Type::Operator;
	m_pcDocView = pcInDocView;
}

H3DF::WindowKey & KERNEL::Command::CommandImpl::Window()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

const H3DF::WindowKey & KERNEL::Command::CommandImpl::Window() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

H3DF::View & KERNEL::Command::CommandImpl::View()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

const H3DF::View & KERNEL::Command::CommandImpl::View() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

void KERNEL::Command::CommandImpl::Copy(CommandImpl * pcInThat)
{
	DEBUG_VALID(pcInThat);

	m_eType = pcInThat->m_eType;
	m_pcDocView = pcInThat->m_pcDocView;
}

Signal::Delivery & KERNEL::Command::CommandImpl::Delivery()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

const Signal::Delivery & KERNEL::Command::CommandImpl::Delivery() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>((DocViewImpl *)m_pcDocView->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

DocView & KERNEL::Command::CommandImpl::GetDocView()
{
	DEBUG_VALID(m_pcDocView);
	return *((KERNEL::DocView *)m_pcDocView);
}

const DocView & KERNEL::Command::CommandImpl::GetDocView() const
{
	DEBUG_VALID(m_pcDocView);
	return *m_pcDocView;
}

H3DF::ModelImpl & KERNEL::Command::CommandImpl::GetModelImpl()
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	H3DF::ModelImpl * pcModelImpl = (H3DF::ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	return *pcModelImpl;
}

const H3DF::ModelImpl & KERNEL::Command::CommandImpl::GetModelImpl() const
{
	DEBUG_VALID(m_pcDocView);

	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	H3DF::ModelImpl * pcModelImpl = (H3DF::ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	return *pcModelImpl;
}

// Update하기전에 Hightlight된 것들을 모두 Unhighlight하고, SnapItem을 모두 Reset한다.
void KERNEL::Command::CommandImpl::PrepareUpdate()
{
	GetDocView().Canvas().GetFrontView().SuppressUpdate(true);

	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(GetDocView().GetImpl());
	pcDocViewImpl->Select().UnhighlightEverything();
	pcDocViewImpl->Select().ResetSnapItems(false);
}

// Update가 완료되면, View를 Update한다.
void KERNEL::Command::CommandImpl::Updated()
{
	GetDocView().Canvas().GetFrontView().SuppressUpdate(false);
	GetDocView().Canvas().GetFrontView().Update();
}


