#include "StdAfx.h"

#include "ApplicationWindowOption.h"

namespace H3DF
{
	class ApplicationWindowOptionsKitPrivate : public Impl
	{
	public:
		ApplicationWindowOptionsKitPrivate() { m_eType = H3DF::Type::ApplicationWindowOptionsKit; }

		void Copy(ApplicationWindowOptionsKitPrivate * pcInThat) {
			m_eDriver = pcInThat->m_eDriver;
		}

		Window::Driver m_eDriver = Window::Driver::Default3D;
	};
}

using namespace H3DF;

//== ApplicationWindowOptionsKit ===================================================================

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit()
{
	m_pcImpl = new ApplicationWindowOptionsKitPrivate();
}

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit(ApplicationWindowOptionsKit const & cInThat)
{
	m_pcImpl = new ApplicationWindowOptionsKitPrivate();
	Set(cInThat);
}

void H3DF::ApplicationWindowOptionsKit::Set(ApplicationWindowOptionsKit const & cInThat)
{
	ApplicationWindowOptionsKitPrivate * pcImpl = (ApplicationWindowOptionsKitPrivate *)m_pcImpl;
	ApplicationWindowOptionsKitPrivate * pcInThatImpl = (ApplicationWindowOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ApplicationWindowOptionsKit const & H3DF::ApplicationWindowOptionsKit::operator = (ApplicationWindowOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::ApplicationWindowOptionsKit::Show(ApplicationWindowOptionsKit & cOutKit) const
{
	ApplicationWindowOptionsKitPrivate * pcImpl = (ApplicationWindowOptionsKitPrivate *)m_pcImpl;
	ApplicationWindowOptionsKitPrivate * pcOutKitImpl = (ApplicationWindowOptionsKitPrivate *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::SetDriver(Window::Driver eInDriver)
{
	ApplicationWindowOptionsKitPrivate * pcImpl = (ApplicationWindowOptionsKitPrivate *)m_pcImpl;
	pcImpl->m_eDriver = eInDriver;
	return *this;
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::UnsetDriver()
{
	ApplicationWindowOptionsKitPrivate * pcImpl = (ApplicationWindowOptionsKitPrivate *)m_pcImpl;
	//pcImpl->m_eDriver = Window::Driver::Undefined;
	return *this;
}