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
		class OperatorImpl : public Impl
		{
		public:
			OperatorImpl(const DocView * pcInDocView);

			H3DF::WindowKey & Window();
			const H3DF::WindowKey & Window() const;

			H3DF::View & View();
			const H3DF::View & View() const;

			void Copy(OperatorImpl * pcInThat);

			Signal::Delivery & Delivery();
			const Signal::Delivery & Delivery() const;

			DocView & GetDocView();
			const DocView & GetDocView() const;

		private:
			const DocView * m_pcDocView = nullptr;
		};
	}
}