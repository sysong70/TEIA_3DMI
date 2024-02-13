#pragma once

#include <3DF/3DF.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class ModelTreeItem;
		class Select;

		class ModelPanel : public OperatorBase
		{
		public:
			ModelPanel(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);
			void Initialize(CString strFilePathName);
			void SetSelect(Select * pcInSelect);

			void Signal(Json::Object & cInObject);

			void SetSelectItem(H3DF::SelectionItem & cSelItem);

		protected:
			CString GetKeyName(HC_KEY nInKey);

			void OnItemExpandedSignal(Json::Object & cInObject);

			void UserInterfaceItemExpanded(ModelTreeItem * pcInItem);

			void OnItemSelectedSignal(Json::Object& cInObject);
			void OnItemCheckedSignal(Json::Object & cInObject);
		};
	}
}	