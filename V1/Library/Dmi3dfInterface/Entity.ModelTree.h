#pragma once

#include "3DF/3DF.h"
#include "3DF/Object.h"

namespace H3DF
{
	namespace Entity
	{
		enum API_3DF ModelTreeItemStatus
		{
			Normal			= 0x0001,
			End				= 0x0002,
			UiUpdate		= 0x0004,
			Invisible		= 0x0008,
			Solid			= 0x0010,
			Surface			= 0x0020,
			Curve			= 0x0040,
			Point			= 0x0080,
		};

		class API_3DF ModelTreeItem : public Object
		{
		public:
			ModelTreeItem(HC_KEY nInKey);

			HC_KEY KeyValue();
			DWORD & Status();
			ModelTreeItem * Parent();
			std::vector<ModelTreeItem *> & Children();

			ModelTreeItem * AddChild(HC_KEY nInChildKey, bool bHasChild);

			bool ShowChild(H3DF::Key & cKey, ModelTreeItem *& pcOutTreeItem);
			bool ShowChild(HC_KEY nInChildKey, ModelTreeItem *& pcOutTreeItem);
		};

		class API_3DF ModelTree : public Object
		{
		public:
			ModelTree();

			ModelTreeItem * AddItem(HC_KEY nInKey, ModelTreeItem * pcInParentItem, bool bHasChild);

			bool ShowSelectionResult(ModelTreeItem * nInItem, H3DF::SelectionResults & cOutResults);

			ModelTreeItem & Root();

			ModelTreeItem * ModelsGroupItem();
			ModelTreeItem * MeasurementsGroupItem();
			ModelTreeItem * MarkupsGroupItem();

			void SetModelsGroupItem(ModelTreeItem * pcInItem);
			void SetMeasurementsGroupItem(ModelTreeItem * pcInItem);
			void SetMarkupsGroupItem(ModelTreeItem * pcInItem);

			bool ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand);

			bool GetItemName(HC_KEY nInKey, CString & strOutName);
		};
	}
}