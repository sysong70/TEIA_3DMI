#include "StdAfx.h"

#include "Operator.h"
#include "Impl/OperatorImpl.h"

#include <3DF/Window.h>

using namespace KERNEL;

namespace KERNEL
{
	namespace Operator
	{
		class OperatorBaseImpl : public Impl
		{
		public:
			OperatorBaseImpl() { m_eType = KERNEL::Type::Operator; }

			KERNEL::DocViewImpl * m_pcDocViewImpl = nullptr;
		};
	}
}

KERNEL::Operator::OperatorBase::OperatorBase()
{
	m_pcImpl1 = new OperatorBaseImpl();

}
void KERNEL::Operator::OperatorBase::SetDocViewImpl(KERNEL::DocViewImpl * pcInDocViewImpl)
{
	OperatorBaseImpl * pcImpl = (OperatorBaseImpl *)m_pcImpl1;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcDocViewImpl = pcInDocViewImpl;
}

KERNEL::DocViewImpl & KERNEL::Operator::OperatorBase::DocViewImpl()
{
	OperatorBaseImpl * pcImpl = (OperatorBaseImpl *)m_pcImpl1;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pcDocViewImpl;
}