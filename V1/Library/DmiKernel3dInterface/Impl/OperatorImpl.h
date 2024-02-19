#pragma once

#include "../Kernel.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

namespace KERNEL
{
	class DocView;

	namespace Operator
	{
		class OperatorImpl : public Impl
		{
		public:
			OperatorImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			H3DF::WindowKey & Window();
			const H3DF::WindowKey & Window() const;

			H3DF::View & View();
			const H3DF::View & View() const;

			void SetDocView(const DocView * pcInDocView);
			void SetView(const H3DF::View * pcInView);

			void Copy(OperatorImpl * pcInThat);

			Signal::Delivery & Delivery();
			const Signal::Delivery & Delivery() const;

		private:
			const H3DF::View * m_pcView = nullptr;
			const Signal::Delivery * m_pcDelivery = nullptr;
			const KERNEL::DocView * m_pcDocView = nullptr;
		};
	}
}