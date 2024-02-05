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
			void OnItemExpandedSignal(Json::Object & cInObject);
			void ModelGroupItemExpanded();
			void MeasurementsGroupItemExpanded();
			void MarkupsGroupItemExpanded();

			void ItemExpanded(ModelTreeItem * pcInItem, ModelTreeItem * pcInParentItem = nullptr);

			void OnItemClickedSignal(Json::Object & cInObject);
			void OnItemCheckedSignal(Json::Object & cInObject);
		};
	}
}	