#include "StdAfx.h"

#include "ApplicationWindowOption.h"

using namespace H3DF;

namespace H3DF
{
	class ApplicationWindowOptionsKitImpl : public Impl
	{
	public:
		ApplicationWindowOptionsKitImpl() { m_eType = H3DF::Type::ApplicationWindowOptionsKit; }

		void Copy(ApplicationWindowOptionsKitImpl * pcInThat) {
			m_eDriver = pcInThat->m_eDriver;
		}

		Window::Driver m_eDriver = Window::Driver::Default3D;
	};
}

//== ApplicationWindowOptionsKit ===================================================================

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit()
{
	m_pcImpl = new ApplicationWindowOptionsKitImpl();
}

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit(ApplicationWindowOptionsKit const & cInThat)
{
	m_pcImpl = new ApplicationWindowOptionsKitImpl();
	Set(cInThat);
}

void H3DF::ApplicationWindowOptionsKit::Set(ApplicationWindowOptionsKit const & cInThat)
{
	ApplicationWindowOptionsKitImpl * pcImpl = (ApplicationWindowOptionsKitImpl *)m_pcImpl;
	ApplicationWindowOptionsKitImpl * pcInThatImpl = (ApplicationWindowOptionsKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ApplicationWindowOptionsKit const & H3DF::ApplicationWindowOptionsKit::operator = (ApplicationWindowOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::ApplicationWindowOptionsKit::Show(ApplicationWindowOptionsKit & cOutKit) const
{
	ApplicationWindowOptionsKitImpl * pcImpl = (ApplicationWindowOptionsKitImpl *)m_pcImpl;
	ApplicationWindowOptionsKitImpl * pcOutKitImpl = (ApplicationWindowOptionsKitImpl *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::SetDriver(Window::Driver eInDriver)
{
	ApplicationWindowOptionsKitImpl * pcImpl = (ApplicationWindowOptionsKitImpl *)m_pcImpl;
	pcImpl->m_eDriver = eInDriver;
	return *this;
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::UnsetDriver()
{
	ApplicationWindowOptionsKitImpl * pcImpl = (ApplicationWindowOptionsKitImpl *)m_pcImpl;
	//pcImpl->m_eDriver = Window::Driver::Undefined;
	return *this;
}