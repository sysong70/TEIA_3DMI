#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "3DF.h"

#include <vector>

OPEN_3DF_NAMESPACE

//== Select 관련 Class ==============================================================================

class MultiSelectManager;

class  QualifiedKey
{
public:
	QualifiedKey(HC_KEY key, int length, HC_KEY * includeList)
	{
		m_nKey = key;
		m_nIncludeListCount = length;
		m_pnIncludeList = new HC_KEY(length);
		for(int i = 0; i < length; ++i)
			m_pnIncludeList[i] = includeList[i];
	}
	QualifiedKey()
	{
		m_pnIncludeList = 0;
		m_nKey = HC_ERROR_KEY;
		m_nIncludeListCount = 0;
	}

	~QualifiedKey()
	{
		delete[] m_pnIncludeList;
	}

	void Select(HBaseView * view);
	HC_KEY GetKey() { return m_nKey; }
	HC_KEY GetKeyHash() { return m_nKey; }

	void SetKey(HC_KEY key) { m_nKey = key; }
	HC_KEY * m_pnIncludeList;
	int  m_nIncludeListCount;
	HC_KEY m_nKey;
};

class MultiSelectItem
{
public:
	MultiSelectItem(HC_KEY key);
	~MultiSelectItem();
	void AddAssociatedKey(HC_KEY key, int length = 0, HC_KEY * include_list = 0);
	void AddAssociatedSelectItem(HC_KEY key, int length = 0, HC_KEY * include_list = 0);
	void MultiSelect(MultiSelectManager * msm, HBaseView * view);
	QualifiedKey m_Key;
	struct vlist_s * m_pcAssociatedKeyList;
	struct vlist_s * m_pcAssociatedSelectItemList;

};

class  MultiSelectManager
{
public:
	MultiSelectManager();
	~MultiSelectManager();

	MultiSelectItem * CreateMultiSelectItem(HC_KEY key);
	MultiSelectItem * GetMultiSelectItem(HC_KEY key);
	HC_KEY MultiSelect(HC_KEY key, HBaseView * view);

	struct vhash_s * m_pcSelectItemHash;
	struct vlist_s * m_pcSelectItemList;
	bool m_bHasItems;
};

//== Clash 관련 Class ===============================================================================

class ClashItem
{
public:
	ClashItem(HC_KEY s1, HC_KEY s2, std::vector<HC_KEY> const & p1, std::vector<HC_KEY> const & p2)
		: m_shell1(s1)
		, m_shell2(s2)
		, m_path1(p1)
		, m_path2(p2)
		, m_bHardClash(true)
	{}
	HC_KEY shell1() const {
		return m_shell1;
	}
	HC_KEY shell2() const {
		return m_shell2;
	}
	std::vector<HC_KEY> fullPath1() const {
		return m_path1;
	}
	std::vector<HC_KEY> fullPath2() const {
		return m_path2;
	}
	std::vector<HC_KEY> inclPath1() const {
		return getInclPath(m_path1);
	}
	std::vector<HC_KEY> inclPath2() const {
		return getInclPath(m_path2);
	}
	bool hardClash() const {
		return m_bHardClash;
	}
	bool & hardClash() {
		return m_bHardClash;
	}
private:
	std::vector<HC_KEY> getInclPath(std::vector<HC_KEY> const & path) const {
		std::vector<HC_KEY> inclPath;
		for(size_t i = 0; i < path.size(); ++i) {
			HC_KEY key = path[i];
			char type[32];
			HC_Show_Key_Type(key, type);
			if(strcmp(type, "include") == 0) {
				inclPath.push_back(key);
			}
		}
		return inclPath;
	}
private:
	HC_KEY m_shell1;
	HC_KEY m_shell2;
	std::vector<HC_KEY> m_path1;
	std::vector<HC_KEY> m_path2;
	bool m_bHardClash;
};

class SelectionControl : public HSelectionSet
{
public:
	SelectionControl(HBaseView * pcView, bool bReferenceSelection = false);
	~SelectionControl();

	// overloaded virtuals
	void Init() override;
	//void Select(HC_KEY key, const char * segpath, HC_KEY include_key, HC_KEY includer_key, bool emit_message = true) = 0 ;

private:
	int	m_nSelectLevel;

	struct vlist_s * m_pcSelection = nullptr;
	bool m_bShowFacesAsLines = false;
};

CLOSE_3DF_NAMESPACE