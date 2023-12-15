#pragma once

#include "../Kernel.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

namespace KERNEL
{
	namespace Operator
	{
		class OperatorPrivate : public PrivateImpl
		{
		public:
			OperatorPrivate(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			H3DF::WindowKey & Window();
			const H3DF::WindowKey & Window() const;

			H3DF::View & View();
			const H3DF::View & View() const;

			void SetView(const H3DF::View * pcInView);

			void Copy(OperatorPrivate * pcInThat);

			Signal::Delivery & Delivery();
			const Signal::Delivery & Delivery() const;

		private:
			const H3DF::View * m_pcView = nullptr;
			const Signal::Delivery * m_pcDelivery = nullptr;
		};
	}
}