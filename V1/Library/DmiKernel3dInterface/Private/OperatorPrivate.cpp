#include "StdAfx.h"

#include "OperatorPrivate.h"

#include <3DF/Window.h>
#include <3DF.View.h>

using namespace KERNEL;

//== Visual Effects class ==========================================================================

KERNEL::Operator::OperatorPrivate::OperatorPrivate(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	DEBUG_VALID(pcInView);

	m_eType = KERNEL::Type::Operator;
	m_pcView = pcInView;
	m_pcDelivery = pcInDelivery;
}

H3DF::WindowKey & KERNEL::Operator::OperatorPrivate::Window()
{
	DEBUG_VALID(m_pcView);
	return m_pcView->GetWindowKey();
}

const H3DF::WindowKey & KERNEL::Operator::OperatorPrivate::Window() const
{
	DEBUG_VALID(m_pcView);
	return m_pcView->GetWindowKey();
}

H3DF::View & KERNEL::Operator::OperatorPrivate::View()
{
	DEBUG_VALID(m_pcView);
	return *(H3DF::View *)m_pcView;
}

const H3DF::View & KERNEL::Operator::OperatorPrivate::View() const
{
	DEBUG_VALID(m_pcView);
	return *m_pcView;
}

void KERNEL::Operator::OperatorPrivate::SetView(const H3DF::View * pcInView)
{
	m_pcView = pcInView;
}

void KERNEL::Operator::OperatorPrivate::Copy(OperatorPrivate * pcInThat)
{
	DEBUG_VALID(pcInThat);

	m_eType = pcInThat->m_eType;
	SetView(&pcInThat->View());
	m_pcDelivery = &pcInThat->Delivery();
}

Signal::Delivery & KERNEL::Operator::OperatorPrivate::Delivery()
{
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & KERNEL::Operator::OperatorPrivate::Delivery() const
{
	return *m_pcDelivery;
}