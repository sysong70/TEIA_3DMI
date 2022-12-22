#include "stdafx.h"

#include "DmiHpsComponentTree.h"

#include "../DmiHpsUtility.h"

#include <Common_Define.h>

HPS::ComponentTreeItemPtr DmiHpsComponentTreeItem::AddChild(HPS::Component const & cInComponent, HPS::ComponentTree::ItemType cInType)
{
	CString strOwnerTitle = DmiHps::ToString(GetTitle());
	HPS::ComponentTree::ItemType eOwnerItemType = GetItemType();

	auto pcChild = std::make_shared<DmiHpsComponentTreeItem>(GetTree(), cInComponent, cInType);
	m_vpcComponentVector.push_back(pcChild);

	HPS::UTF8 strTitle = pcChild->GetTitle();
	if(strTitle.Empty()) {
		strTitle = "Unnamed";
	}

	HPS::ComponentTree::ItemType eItemType = pcChild->GetItemType();

	HPS::WCharArray wstr;
	strTitle.ToWStr(wstr);

// 	TVINSERTSTRUCT is;
// 	is.hParent = GetTreeItem();
// 	is.hInsertAfter = TVI_LAST;
// 	is.item.mask = TVIF_CHILDREN | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
// 	is.item.cChildren = pcChild->HasChildren();
// 	is.item.lParam = (LPARAM) pcChild.get();
// 	is.item.pszText = (LPTSTR) wstr.data();
// 	is.item.iImage = GetImageIndex(*pcChild);
// 	is.item.iSelectedImage = is.item.iImage;
// 
// 	pcChild->SetTreeItem(GetTreeCtrl()->InsertItem(&is));

	return pcChild;
}

void DmiHpsComponentTreeItem::Expand()
{
	ExpandChild(true);

	//HPS::ComponentTreeItem::Expand();

/*
	if(false == m_bExpendedFlag) {
		m_bExpendedFlag = true;
		for(auto pcChild : m_vChildComponentVector) {
			pcChild->
			pcChild->Expand();
		}
	}
*/
/*
	CTreeCtrl * treeCtrl = GetTreeCtrl();
	if(treeCtrl != nullptr)
	{
		if(GetTreeItem() == nullptr)
			HPS::ComponentTreeItem::Expand();
		else
		{
			bool is_expanded = ((treeCtrl->GetItemState(GetTreeItem(), TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
			if(is_expanded == false)
			{
				HPS::ComponentTreeItem::Expand();
				treeCtrl->SetItemState(treeItem, TVIS_EXPANDED, TVIS_EXPANDED);
				// this will trigger the TVN_ITEMEXPANDING message, which will in turn trigger this function
				// so to avoid stack overflow, we set the item state as TVIS_EXPANDED to avoid invoking this repeatedly
				treeCtrl->Expand(GetTreeItem(), TVE_EXPAND);
			}
			else
				treeCtrl->SetItemState(GetTreeItem(), 0, TVIS_EXPANDED);
		}
	}
* /*/
}

void DmiHpsComponentTreeItem::ExpandChild(bool bExpandChildFlag)
{
	HPS::ComponentTreeItem::Expand();

// 	if(true == bExpandChildFlag) {
// 		m_bExpendedFlag = true;
// 		for(auto pcChild : m_vpcComponentVector) {
// 			pcChild->ExpandChild(false);
// 		}
// 	}
}

void DmiHpsComponentTreeItem::Collapse()
{
	HPS::ComponentTreeItem::Collapse();
	// must remove all child elements since they will be reinserted
/*
	CTreeCtrl * treeCtrl = GetTreeCtrl();
	if(treeCtrl != nullptr)
		treeCtrl->Expand(GetTreeItem(), TVE_COLLAPSE | TVE_COLLAPSERESET);*/
}

void DmiHpsComponentTreeItem::GetJsonObject(Json::Object & cRetObject)
{
	// 1. Title 저장
	// #Require_convert_new_version
/*
	cRetObject.SetString("Title", DmiHps::ToString(GetTitle()));
	cRetObject.SetDwordPtr("Type", GetComponentType(*this));
	cRetObject.SetBoolean("HasChildren", HasChildren());
	cRetObject.SetDwordPtr("CompPtr", (DWORD_PTR) this);

	// 2. Child가 있는 경우 Child Array 생성
	Json::Array & acChildArray = cRetObject.GenArray("Child");

	for(auto pcChild : m_vpcComponentVector) {
		Json::Object & cChildObject = acChildArray.GenObject();
		pcChild->GetJsonObject(cChildObject);
	}*/
}

int DmiHpsComponentTreeItem::GetComponentType(HPS::ComponentTreeItem const & cItem)
{
	// #Require_convert_new_version
/*
	DmiHps::ItemType eItemType = DmiHps::ItemType::Unknown;

	HPS::ComponentTreePtr pcTree = GetTree();

	if(nullptr != pcTree)
	{
		switch(cItem.GetItemType())
		{
			case HPS::ComponentTree::ItemType::ExchangeViewGroup:
			case HPS::ComponentTree::ItemType::ExchangeAnnotationViewGroup:
			{
				eItemType = DmiHps::ItemType::ViewGroup;
			}	break;

			case HPS::ComponentTree::ItemType::ExchangePMIGroup:
			{
				eItemType = DmiHps::ItemType::PMIGroup;
			}	break;

			case HPS::ComponentTree::ItemType::ExchangeModelGroup:
			{
				eItemType = DmiHps::ItemType::RIGeometrySet;
			}	break;

			case HPS::ComponentTree::ItemType::DWGModelFile:
			case HPS::ComponentTree::ItemType::ExchangeModelFile:
			{
				eItemType = DmiHps::ItemType::ModelFile;
			}	break;

			case HPS::ComponentTree::ItemType::ExchangeComponent:
			{
				switch(cItem.GetComponent().GetComponentType())
				{
					case HPS::Component::ComponentType::ExchangeProductOccurrence:
					{
						HPS::ComponentArray subcomponents = cItem.GetComponent().GetSubcomponents();
						auto it = std::find_if(subcomponents.begin(), subcomponents.end(),
							[] (HPS::Component const & comp) { return comp.GetComponentType() == HPS::Component::ComponentType::ExchangeProductOccurrence; });
						if(it == subcomponents.end())
							eItemType = DmiHps::ItemType::ProductStructureWithoutChildren;
						else
							eItemType = DmiHps::ItemType::ProductStructureWithChildren;
					}	break;

					case HPS::Component::ComponentType::ExchangeRISet:
					{
						eItemType = DmiHps::ItemType::RIGeometrySet;
					}	break;

					case HPS::Component::ComponentType::ExchangeRIPlane:
					{
						eItemType = DmiHps::ItemType::RIPlane;
					}	break;

					case HPS::Component::ComponentType::ExchangeRIDirection:
					{
						eItemType = DmiHps::ItemType::RIDirection;
					}	break;

					case HPS::Component::ComponentType::ExchangeRICoordinateSystem:
					{
						eItemType = DmiHps::ItemType::RICoordinateSystem;
					}	break;

					case HPS::Component::ComponentType::ExchangeRIBRepModel:
					case HPS::Component::ComponentType::ExchangeRIPolyBRepModel:
					{
						eItemType = DmiHps::ItemType::RISolid;
					}	break;

					case HPS::Component::ComponentType::ExchangeRICurve:
					case HPS::Component::ComponentType::ExchangeRIPolyWire:
					{
						eItemType = DmiHps::ItemType::RICurve;
					}	break;

					case HPS::Component::ComponentType::ExchangeRIPointSet:
					{
						eItemType = DmiHps::ItemType::RIPointCloud;
					}	break;

					case HPS::Component::ComponentType::ExchangeView:
					{
						if(HPS::BooleanMetadata(cItem.GetComponent().GetMetadata("IsAnnotationCapture")).GetValue() == true)
							eItemType = DmiHps::ItemType::AnnotationView;
						else
							eItemType = DmiHps::ItemType::View;
					}	break;

					case HPS::Component::ComponentType::ExchangePMI:
					case HPS::Component::ComponentType::ExchangePMIText:
					case HPS::Component::ComponentType::ExchangePMIRichText:
					{
						eItemType = DmiHps::ItemType::PMIText;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIGDT:
					{
						eItemType = DmiHps::ItemType::PMITolerance;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIRoughness:
					{
						eItemType = DmiHps::ItemType::PMIRoughness;
					}	break;

					case HPS::Component::ComponentType::ExchangePMILineWelding:
					{
						eItemType = DmiHps::ItemType::PMILineWelding;
					}	break;

					case HPS::Component::ComponentType::ExchangePMISpotWelding:
					{
						eItemType = DmiHps::ItemType::PMISpotWelding;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIDatum:
					{
						eItemType = DmiHps::ItemType::PMIDatum;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIDimension:
					{
						eItemType = DmiHps::ItemType::PMIDimensionDistance;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIBalloon:
					{
						eItemType = DmiHps::ItemType::PMIBalloon;
					}	break;

					case HPS::Component::ComponentType::ExchangePMICoordinate:
					{
						eItemType = DmiHps::ItemType::PMICoordinate;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIFastener:
					{
						eItemType = DmiHps::ItemType::PMIFastener;
					}	break;

					case HPS::Component::ComponentType::ExchangePMILocator:
					{
						eItemType = DmiHps::ItemType::PMILocator;
					}	break;

					case HPS::Component::ComponentType::ExchangePMIMeasurementPoint:
					{
						eItemType = DmiHps::ItemType::PMIMeasurementPoint;
					}	break;

					case HPS::Component::ComponentType::ExchangeDrawingModel:
					{
						eItemType = DmiHps::ItemType::DrawingModel;
					}	break;

					case HPS::Component::ComponentType::ExchangeDrawingSheet:
					{
						eItemType = DmiHps::ItemType::DrawingSheet;
					}	break;

					case HPS::Component::ComponentType::ExchangeDrawingView:
					{
						eItemType = DmiHps::ItemType::DrawingView;
					}	break;

					default:
					{
						eItemType = DmiHps::ItemType::Unknown;
					}	break;
				}
			}	break;

			case HPS::ComponentTree::ItemType::ParasolidModelFile:
			{
				eItemType = DmiHps::ItemType::ParasolidModel;
			}	break;

			case HPS::ComponentTree::ItemType::ParasolidComponent:
			{
				switch(cItem.GetComponent().GetComponentType())
				{
					case HPS::Component::ComponentType::ParasolidAssembly:
					{
						eItemType = DmiHps::ItemType::ParasolidAssembly;
					}	break;

					case HPS::Component::ComponentType::ParasolidTopoBody:
					{
						eItemType = DmiHps::ItemType::ParasolidBody;
					}	break;

					case HPS::Component::ComponentType::ParasolidInstance:
					{
						HPS::Component::ComponentType component_type = cItem.GetComponent().GetSubcomponents()[0].GetComponentType();
						if(component_type == HPS::Component::ComponentType::ParasolidAssembly)
							eItemType = DmiHps::ItemType::ParasolidAssembly;
						else if(component_type == HPS::Component::ComponentType::ParasolidTopoBody)
							eItemType = DmiHps::ItemType::ParasolidBody;
					} break;

					default:
					{
						eItemType = DmiHps::ItemType::Unknown;
					}	break;
				}
			}	break;

			case HPS::ComponentTree::ItemType::DWGComponent:
			{
				HPS::Component::ComponentType component_type = cItem.GetComponent().GetComponentType();
				switch(component_type)
				{
					case HPS::Component::ComponentType::DWGBlockTable:
					case HPS::Component::ComponentType::DWGLayerTable:
					case HPS::Component::ComponentType::DWGBlockTableRecord:
					{
						eItemType = DmiHps::ItemType::RIGeometrySet;
						break;
					}
					case HPS::Component::ComponentType::DWGLayer:
					{
						eItemType = DmiHps::ItemType::DrawingSheet;
						break;
					}
					case HPS::Component::ComponentType::DWGEntity:
					{
						eItemType = DmiHps::ItemType::RISolid;
						break;
					}
					case HPS::Component::ComponentType::DWGLayout:
					{
						eItemType = DmiHps::ItemType::AnnotationView;
						break;
					}
				}

			} break;

			default:
			{
				eItemType = DmiHps::ItemType::Unknown;
			}	break;
		}
	}

	return (int) eItemType;
*/

	return -1;
}


void DmiHpsComponentTreeItem::OnHighlight(HPS::HighlightOptionsKit const & in_options)
{
	int i = 0;
}
