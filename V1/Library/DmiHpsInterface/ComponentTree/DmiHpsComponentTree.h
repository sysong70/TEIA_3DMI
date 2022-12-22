#pragma once

#include <memory>

#include <Json.h>

class DmiHpsComponentTree;
class DmiHpsComponentTreeItem;

using DmiHpsComponentTreePtr = std::shared_ptr<DmiHpsComponentTree>;
using DmiHpsComponentTreeItemPtr = std::shared_ptr<DmiHpsComponentTreeItem>;

class DmiHpsComponentTree : public HPS::ComponentTree
{
public:
	DmiHpsComponentTree(HPS::Canvas const & cInCanvas)
		: HPS::ComponentTree(cInCanvas) {}

	/*CTreeCtrl * GetTreeCtrl();
	CHPSModelBrowserPane * GetModelBrowserPane() { return browser; }

	virtual void Flush() override
	{
		HPS::ComponentTree::Flush();
		GetTreeCtrl()->DeleteAllItems();
	}
*/
// 
// private:
// 	CHPSModelBrowserPane * browser;

};

class DmiHpsComponentTreeItem : public HPS::ComponentTreeItem
{
public:
	DmiHpsComponentTreeItem(HPS::ComponentTreePtr const & in_tree, HPS::CADModel const & in_cad_model)
		: HPS::ComponentTreeItem(in_tree, in_cad_model) {}

	DmiHpsComponentTreeItem(HPS::ComponentTreePtr const & in_tree, HPS::Component const & in_component, HPS::ComponentTree::ItemType in_type)
		: HPS::ComponentTreeItem(in_tree, in_component, in_type) {}

	HPS::ComponentTreeItemPtr AddChild(HPS::Component const & cInComponent, HPS::ComponentTree::ItemType cInType) override;

	void Expand() override;

	void Collapse() override;

	void GetJsonObject(Json::Object & cRetObject);

	int GetComponentType(HPS::ComponentTreeItem const & cItem);

 	void OnHighlight(HPS::HighlightOptionsKit const & in_options) override;
// 	virtual void OnUnhighlight(HPS::HighlightOptionsKit const & in_options) override;
// 
// 	virtual void OnHide() override;
// 	virtual void OnShow() override;

	//inline CHPSModelBrowserPane * GetModelBrowserPane() const { return std::static_pointer_cast<DmiHpsComponentTree>(GetTree())->GetModelBrowserPane(); }

	bool m_bExpendedFlag = false;

	//DmiHpsComponentTreeItemPtr GetTreeCtrlItem() { return m_pcTreeCtrlItem; }

private:
	void ExpandChild(bool bExpandChildFlag);

// 	inline void SetTreeItem(HTREEITEM in_item) { treeItem = in_item; }
// 	inline HTREEITEM GetTreeItem() const { return treeItem; }
// 	inline CTreeCtrl * GetTreeCtrl() const { return std::static_pointer_cast<DmiHpsComponentTree>(GetTree())->GetTreeCtrl(); }
// 	int GetImageIndex(HPS::ComponentTreeItem const & item);
// 	int GetHiddenImageIndex(HPS::ComponentTreeItem const & item);
// 
// 	HTREEITEM treeItem{};

	//DmiHpsComponentTreeItemPtr m_pcTreeCtrlItem;


	std::vector<DmiHpsComponentTreeItemPtr> m_vpcComponentVector;
};