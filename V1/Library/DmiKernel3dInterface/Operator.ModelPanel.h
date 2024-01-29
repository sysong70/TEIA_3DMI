#pragma once

#include <3DF/3DF.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class ModelPanel : public OperatorBase
		{
		public:
			ModelPanel(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Initialize(CString strFilePathName);

			void Signal(Json::Object & cInObject);
		};
	}
}	