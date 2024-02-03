#pragma once

#include <3DF/3DF.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Select;

		class ModelPanel : public OperatorBase
		{
		public:
			ModelPanel(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Initialize(CString strFilePathName);

			void SetSelect(Select * pcInSelect);

			void Signal(Json::Object & cInObject);
			

		protected:
			void ItemExpandedSignal(Json::Object & cInObject);
			void ModelGroupItemExpanded();
			void MeasurementsGroupItemExpanded();
			void MarkupsGroupItemExpanded();

			void ItemExpanded(DWORD_PTR nInItemKey, DWORD_PTR nInParentItem = 0);
			//void IncludeExpanded1(DWORD_PTR nInItem, DWORD_PTR nInParentItem = 0);

			void SelChangedSignal(Json::Object & cInObject);
		};
	}
}	