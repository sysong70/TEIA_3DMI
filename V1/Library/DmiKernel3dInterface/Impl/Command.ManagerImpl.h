#pragma once

#include "../Kernel.h"

#include "../Kernel.Application.h"
#include "../Kernel.DocView.h"

// #include <3DF/3DF.h>
// #include <3DF/Math.h>

//#include <Sprocket/3DF.Application.h>
//#include <Sprocket/Impl//ModelImpl.h>

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

namespace KERNEL
{
	namespace Command
	{
		class ManagerImpl : public Impl
		{
		public:
			void Copy(ManagerImpl * that)
			{
				m_cApplication = that->m_cApplication;
			}

			KERNEL::Application m_cApplication;
			int m_nThreadFileOpenViewId = -1;

			KERNEL::DocView * GetDocView(int nId);

			void ViewMouseSignal(Json::Object & cInObject, int nViewId);
			void ViewKeyboardSignal(Json::Object & cInObject, int nViewId);
			void ViewExecuteCommand(Json::Object & cInObject, int nViewId);
		};
	}
}