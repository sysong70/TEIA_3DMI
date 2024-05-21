#pragma once

#include "../Kernel.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include <Sprocket/Impl//ModelImpl.h>

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

namespace KERNEL
{
	namespace Command
	{
		class CommandImpl : public Impl
		{
		public:
			CommandImpl(const Session * pcInSession);

			H3DF::WindowKey & Window();
			const H3DF::WindowKey & Window() const;

			H3DF::View & View();
			const H3DF::View & View() const;

			void Copy(CommandImpl * pcInThat);

			Signal::Delivery & Delivery();
			const Signal::Delivery & Delivery() const;

			Session & GetSession();
			const Session & GetSession() const;

			H3DF::ModelImpl & GetModelImpl();
			const H3DF::ModelImpl & GetModelImpl() const;

			// Update하기전에 Hightlight된 것들을 모두 Unhighlight하고, SnapItem을 모두 Reset한다.
			void PrepareUpdate();
			// Update가 완료되면, View를 Update한다.
			void Updated();

		private:
			const Session * m_pcSession = nullptr;
		};
	}
}