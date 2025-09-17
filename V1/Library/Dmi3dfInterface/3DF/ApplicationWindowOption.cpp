#include "StdAfx.h"

#include "Impl/3DF.Impl.h"

#include "ApplicationWindowOption.h"

using namespace H3DF;

namespace H3DF
{
	class ApplicationWindowOptionsKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ApplicationWindowOptionsKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ApplicationWindowOptionsKitImpl * pcInSource) {
			if (nullptr == pcInSource) {
				DEBUG_VALID(pcInSource);
				return;
			}

			m_eDriver = pcInSource->m_eDriver;
		}

		Window::Driver m_eDriver = Window::Driver::Default3D;
	};
}

//== ApplicationWindowOptionsKit ===================================================================

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit()
{
	m_pcImpl = std::make_unique<ApplicationWindowOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ApplicationWindowOptionsKit::ApplicationWindowOptionsKit(ApplicationWindowOptionsKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

ApplicationWindowOptionsKit const & H3DF::ApplicationWindowOptionsKit::operator = (ApplicationWindowOptionsKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ApplicationWindowOptionsKit::Show(ApplicationWindowOptionsKit & cOutKit) const
{
	auto pcImpl = static_cast<ApplicationWindowOptionsKitImpl *>(m_pcImpl.get());
	auto pcOutKitImpl = static_cast<ApplicationWindowOptionsKitImpl *>(cOutKit.m_pcImpl.get());

	pcOutKitImpl->Copy(pcImpl);
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::SetDriver(Window::Driver eInDriver)
{
	auto pcImpl = static_cast<ApplicationWindowOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->m_eDriver = eInDriver;
	return *this;
}

ApplicationWindowOptionsKit & H3DF::ApplicationWindowOptionsKit::UnsetDriver()
{
	auto pcImpl = static_cast<ApplicationWindowOptionsKitImpl *>(m_pcImpl.get());
	//pcImpl->m_eDriver = Window::Driver::Undefined;
	return *this;
}