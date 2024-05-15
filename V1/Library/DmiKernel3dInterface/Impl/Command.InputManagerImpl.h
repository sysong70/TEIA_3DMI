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
		class InputKitImpl : public Impl
		{
		public:
			void Copy(InputKitImpl * that)
			{
				m_aPoints = that->m_aPoints;
				m_aStrings = that->m_aStrings;
			}

			H3DF::PointArray m_aPoints;
			H3DF::StringArray m_aStrings;
		};
	}
}