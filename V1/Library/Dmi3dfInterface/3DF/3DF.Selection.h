#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "3DF.h"
#include "3DF.Kit.h"
#include "3DF.Control.h"

#include "3DF.Math.h"
#include "3DF.Point.h"

#include <vector>

OPEN_3DF_NAMESPACE

class Selection
{
public:

	// Enumeration of the various selection levels. 
	enum class Level : uint32_t
	{
		Segment,			// Return the owning segment of the selected geometry entities in SelectionResults.
		Entity,				// Return the selected geometry entities in SelectionResults.
		Subentity,			// Return the selected geometry entities and the associated faces, edges and vertices for shells or meshes in SelectionResults.
	};

	// Enumeration of the various selection sorting choices. 
	enum class Sorting : uint32_t
	{
		Off,		// Do not sort selection results.

		// 선택 지점에 대한 화면 공간 근접성에 따라 선택 결과를 정렬합니다.
		// 동점(동일한 근접성을 가진 항목)은 추가로 z 정렬됩니다.
		// 선(및 가장자리와 원호와 같은 모든 선과 같은 형상) 및 마커는 보다 쉽게 선택할 수 있도록 편향되어 있습니다.
		// 이 편향은 근접 계산의 일부이므로 선과 마커가 선택 지점에 더 가까운 다른 지오메트리보다 먼저 정렬될 수 있습니다.
		// Proximity sorting은 SelectByPoint와만 관련이 있습니다.

		// Sort selection results according to the screen-space proximity to the selection point.
		// Ties (items with equal proximity) will be further z-sorted. Note that Lines (and edges and all line-like geometry such as circular arcs)
		// and Markers are biased to make them more easily selectable. This biasing is part of the proximity calculation, and it is therefore possible
		// that Lines and Markers are sorted ahead of other geometry that is closer to the selection point.
		// 
		// Proximity sorting is only relevant for SelectByPoint. 
		Proximity,
		ZSorting,	// Sort selection results from front to back.
		Default		// Use Proximity sorting for SelectByPoint. Use ZSorting in all other cases.
	};

	// Enumeration of the various selection algorithms. 
	enum class Algorithm : uint32_t
	{
		// 이 알고리즘은 SelectionResults를 후처리하여 장면의 다른 개체에 의해 가려진 항목을 제거하므로 화면에 표시되는 항목만 선택됩니다.
		//	This algorithm will post-process the SelectionResults to remove items which are obscured by other objects in the scene,
		//	and therefore only items that are visible on the screen will be selected. 
		Visual,

		// 이 알고리즘은 analytic world space에서 선택 테스트를 수행하고 가려짐을 기반으로 SelectionResults를 사후 처리하지 않습니다.
		// 따라서 현재 표시되지 않는 항목은 여전히 SelectionResults에 반환될 수 있습니다.
		//	This algorithm will perform analytic world space selection testing, and will not post-process the SelectionResults based
		//	on obscuration. Therefore, items which are not currently visible may still be returned in the SelectionResults. 
		Analytic
	};

	// Enumeration of the various selection granularities. 
	enum class Granularity : uint32_t
	{
		General,			// Use the most efficient selection determination for curves, edges and lines.
		Detailed			// Use the most accurate selection determination for curves, edges and lines.
	};

	enum class Bias : uint32_t
	{
		Lines,
		NoLines,
		Markers,
		NoMarkers,
		None
	};

private:
	Selection() {}
};

class SelectionOptionsKit : public Kit
{
public:
	SelectionOptionsKit();
	SelectionOptionsKit(SelectionOptionsKit const & cInThat);

	void Set(SelectionOptionsKit const & cInThat);
	SelectionOptionsKit & operator =(SelectionOptionsKit const & cInThat);

	SelectionOptionsKit & SetProximity(float fInProximity);
	SelectionOptionsKit & SetLevel(Selection::Level eInLevel);
	SelectionOptionsKit & SetInternalLimit(size_t nInLimit);
	SelectionOptionsKit & SetRelatedLimit(size_t nInLimit);
	SelectionOptionsKit & SetSorting(Selection::Sorting eInSorting);
	SelectionOptionsKit & SetAlgorithm(Selection::Algorithm eInAlgorithm);
	SelectionOptionsKit & SetGranularity(Selection::Granularity eInGranularity);
	SelectionOptionsKit & SetBias(Selection::Bias eInBias);

	bool ShowProximity(float & fOutProximity) const;
	bool ShowLevel(Selection::Level & eOutLevel) const;
	bool ShowInternalLimit(size_t & nOutLimit) const;
	bool ShowRelatedLimit(size_t & nOutLimit) const;
	bool ShowSorting(Selection::Sorting & eOutSorting) const;
	bool ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const;
	bool ShowGranularity(Selection::Granularity & eOutGranularity) const;
	bool ShowBias(Selection::Bias & eOutBias) const;
};

class SelectionOptionsControl : public Control
{
public:
	explicit SelectionOptionsControl(WindowKey const & cInWindow);
	SelectionOptionsControl(SelectionOptionsControl const & cInThat);
	~SelectionOptionsControl();

	void Set(SelectionOptionsControl const & cInThat);
	SelectionOptionsControl & operator=(SelectionOptionsControl const & cInThat);

	// https://docs.techsoft3d.com/hps/latest/api_ref/cs/class_h_p_s_1_1_selection_options_control.html?highlight=setproximity#function-HPS.SelectionOptionsControl.SetProximity
	SelectionOptionsControl & SetProximity(float fInProximity);

	SelectionOptionsControl & SetLevel(Selection::Level eInLevel);

	SelectionOptionsControl & SetInternalLimit(size_t nInLimit);
	
	SelectionOptionsControl & SetRelatedLimit(size_t nInLimit);
	
	SelectionOptionsControl & SetSorting(Selection::Sorting eInSorting);
	
	SelectionOptionsControl & SetAlgorithm(Selection::Algorithm eInAlgorithm);
	
	SelectionOptionsControl & SetGranularity(Selection::Granularity eInGranularity);

	// 선택 우선 지정, (Line and Marker)
	SelectionOptionsControl & SetBias(Selection::Bias eInBias);

	SelectionOptionsControl & UnsetProximity();
	SelectionOptionsControl & UnsetLevel();
	SelectionOptionsControl & UnsetInternalLimit();
	SelectionOptionsControl & UnsetRelatedLimit();
	SelectionOptionsControl & UnsetSorting();
	SelectionOptionsControl & UnsetAlgorithm();
	SelectionOptionsControl & UnsetGranularity();
	SelectionOptionsControl & UnsetBias();

private:
	// Private default constructor to prevent instantiation without a window.
	SelectionOptionsControl();
};

class SelectionItem : public Object
{
public:
	// The default constructor creates an uninitialized SelectionItem object.  The Type() function will return Type::None.
	SelectionItem();
	SelectionItem(SelectionItem const & cInThat);
	
	TDF::Type ObjectType() const { return TDF::Type::SelectionItem; };

	void Set(SelectionItem const & cInThat);
	SelectionItem & operator=(SelectionItem const & cInThat);

	bool operator==(SelectionItem const & cInThat) const;
	bool operator!=(SelectionItem const & cInThat) const;

	bool ShowSelectedItem(Key & cOutSelection);

	bool ShowPath(KeyPath & cOutPath) const;

	bool ShowSelectionPosition(WindowPoint & cOutLocation) const;
	bool ShowSelectionPosition(WorldPoint & cOutLocation) const;
};

class SelectionResults : public Object
{
public:
	// The default constructor creates an uninitialized SelectionItem object.  The Type() function will return Type::None.
	SelectionResults();
	SelectionResults(SelectionResults const & cInThat);
	~SelectionResults();

	TDF::Type ObjectType() const { return TDF::Type::SelectionResults; };

	void Set(SelectionResults const & cInThat);
	SelectionResults & operator=(SelectionResults const & cInThat);

	bool operator==(SelectionResults const & cInThat) const;
	bool operator!=(SelectionResults const & cInThat) const;

	void Reset();

	size_t GetCount() const;
	POSITION GetHeadPosition() const;

	SelectionItem * GetHead();
	SelectionItem * GetHead() const;

	SelectionItem * GetAt(POSITION & pcPosition);
	SelectionItem * GetAt(POSITION & pcPosition) const;

	SelectionItem * GetNext(POSITION & pcPosition);
	SelectionItem * GetNext(POSITION & pcPosition) const;

	void RemoveAt(POSITION & pcPosition);
	void RemoveAt(POSITION & pcPosition) const;

	void SetSize(size_t nInSize);

	bool Union(SelectionResults const & cInThat);

	void LeaveType(DWORD nType);
	void RemoveType(DWORD nType);
};

class SelectionControl : public Control
{
public:
	explicit SelectionControl(WindowKey const & cInWindow);
	SelectionControl(SelectionControl const & cInThat);
	~SelectionControl();

	void Set(SelectionControl const & cInThat);
	SelectionControl & operator=(SelectionControl const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::SelectionControl; };

	size_t SelectByPoint(HEventInfo & cEvent, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const;
	size_t SelectByPoint(HEventInfo & cEvent, SelectionResults & cOutResults) const;

	size_t SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const;
	size_t SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const;
	size_t SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const;

private:
	// Private default constructor to prevent instantiation without a window.
	SelectionControl();
};

class DmiSelectionControl : public HSelectionSet
{
public:
	DmiSelectionControl(HBaseView * pcView, bool bReferenceSelection = false);
	~DmiSelectionControl();

	// overloaded virtuals
	void Init() override;

	void Select(HC_KEY key, int num_include_keys, HC_KEY * include_keys, bool emit_message = true) override;

private:
	bool m_bShowFacesAsLines;

	int	m_nSelectLevel;

	struct vlist_s * m_pcSelection;
};


CLOSE_3DF_NAMESPACE