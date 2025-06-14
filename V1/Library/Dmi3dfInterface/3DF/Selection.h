
#pragma once

#include "3DF.h"
#include "KeyPath.h"
#include "Kit.h"
#include "Control.h"

#include "Math.h"
#include "Point.h"

#include <vector>

class HBaseView;

namespace H3DF
{
	class API_3DF Selection
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
			
			Proximity,  // Proximity sorting is only relevant for SelectByPoint. 
			ZSorting,	// Sort selection results from front to back.
			Default 	// Use Proximity sorting for SelectByPoint. Use ZSorting in all other cases.
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

	class API_3DF SelectionOptionsKit : public Kit
	{
	public:
		SelectionOptionsKit();
		SelectionOptionsKit(SelectionOptionsKit const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::SelectionOptionsKit;
		H3DF::Type ObjectType() const { return staticType; };

		SelectionOptionsKit & operator =(SelectionOptionsKit const & cInThat);

		// Sets the selection proximity in centimeters or object-relative-units (ORU), depending
		// on the selection-routine being utilized. The selection proximity augments point-based or
		// shell-based selections by also factoring in the area surrounding the selection-point or
		// selection-shell.
		//
		// For SelectionControl::SelectByPoint, this specifies the radius in
		// centimeters around the selection within which objects will be returned as selected.
		// The value must be positive.
		//
		// For SelectionControl::SelectByShell, this specifies a distance in object-relative-units that determines whether a selection is performed. 
		// A positive proximity value will cause the selection algorithm to perform a selection when the distance between the two bodies is <= proximity, which means the bodies do not have
		// to be touching in order for Visualize to perform a selection.  If the proximity == 0,
		// the bodies must be coincident or penetrating for a selection to occur. If proximity < 0,
		// the shells must penetrate each other by at least that amount before a selection is performed.
		//
		// Selection proximity is not relevant for other selection types.
		//
		// When using HPS::SelectionControl::SelectByShell, false positives or negatives for selection may occur if the proximity and/or selection shells meet any of the following criteria:
		// - The absolute value of a negative proximity is much larger than the actual intersection of the shells.   An example would be a shell that represents a thin plate or a thinly-walled tube, and the specified proximity is larger than the thickness of the plate or tube.
		// - Selection shells ("probes") have vertices with complex intersections
		// - Selection shells ("probes") have concavities, especially multiple adjacent concavities.
		SelectionOptionsKit & SetProximity(float fInProximity);

		SelectionOptionsKit & SetLevel(Selection::Level eInLevel);

		// Sets the internal selection limit. The internal selection limit is the maximum number of subentities for shells and meshes that will be
		// returned if performing subentity selection.
		SelectionOptionsKit & SetInternalLimit(size_t nInLimit);

		// Sets the related selection limit. The related selection limit is the maximum number of items that will be returned as selected when performing
		// a selection. A related selection limit of 0 would result in only the first item getting returned. If the value is
		// greater than 0, this indicates the number of additional items beyond the first to return. The order of these additional
		// items will depend on whether sorting is enabled
		SelectionOptionsKit & SetRelatedLimit(size_t nInLimit);

		// Sets whether to sort selection results. This is only relevant if the related selection limit is greater than 0 (see SetRelatedLimit).
		// Sorting works on an entity level. Subentity components like edges, vertices and faces are not sorted.
		SelectionOptionsKit & SetSorting(Selection::Sorting eInSorting);
		SelectionOptionsKit & SetAlgorithm(Selection::Algorithm eInAlgorithm);
		SelectionOptionsKit & SetGranularity(Selection::Granularity eInGranularity);
		SelectionOptionsKit & SetBias(Selection::Bias eInBias);

		// Sets the starting location at which selection testing will begin.
		// If the selection is being performed from a window, there must be a path from this segment to that window.
		// param: cInStartSegment A segment, that must be a child of the window key, in which to begin selection testing.
		// param: bInScopeOnly If true selections will only occur in the provided scope segment,
		// otherwise selections will occur in subsegments and includes of in_start_segment as well.
		// return: A reference to this SelectionOptionsKit.
		SelectionOptionsKit & SetScope(SegmentKey const & cInStartSegment, bool bInScopeOnly = false);

		// Sets the starting location at which selection testing will begin.
		// If the selection is being performed from a window, there must be a path from this segment to that window.
		// param: cInStartPath A path of segments and includes, leaf to root, from the segment to begin selection testing to the window key.
		// param: bInScopeOnly If true selections will only occur in the provided scope segment,
		// otherwise selections will occur in subsegments and includes of in_start_segment as well.
		// return: A reference to this SelectionOptionsKit.
		SelectionOptionsKit & SetScope(KeyPath const & cInStartPath, bool bInScopeOnly = false);


		bool ShowProximity(float & fOutProximity) const;
		bool ShowLevel(Selection::Level & eOutLevel) const;
		bool ShowInternalLimit(size_t & nOutLimit) const;
		bool ShowRelatedLimit(size_t & nOutLimit) const;
		bool ShowSorting(Selection::Sorting & eOutSorting) const;
		bool ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const;
		bool ShowGranularity(Selection::Granularity & eOutGranularity) const;
		bool ShowBias(Selection::Bias & eOutBias) const;
		bool ShowScope(SegmentKey & cOutStartSegment, bool & bOutScopeOnly) const;
		bool ShowScope(KeyPath & cOutStartPath, bool & bOutScopeOnly) const;
	};

	class API_3DF SelectionOptionsControl : public Control
	{
	public:
		explicit SelectionOptionsControl(WindowKey const & cInWindow);
		SelectionOptionsControl(SelectionOptionsControl const & cInThat);
		virtual ~SelectionOptionsControl();

		static const H3DF::Type staticType = H3DF::Type::SelectionOptionsControl;
		H3DF::Type ObjectType() const { return staticType; };

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
		SelectionOptionsControl() = default;
	};

	class API_3DF SelectionItem : public Object
	{
	public:
		// The default constructor creates an uninitialized SelectionItem object.  The Type() function will return Type::None.
		SelectionItem();
		SelectionItem(SelectionItem const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::SelectionItem;
		H3DF::Type ObjectType() const { return staticType; };

		// 선택된 Item의 Type을 반환합니다.
		H3DF::Type ItemType() const;

		SelectionItem & operator = (SelectionItem const & cInThat);

		bool operator==(SelectionItem const & cInThat) const;
		bool operator!=(SelectionItem const & cInThat) const;

		void Reset();
		void Reset() const;

		bool IsValid();
		bool IsValid() const;

		bool ShowSelectedItem(Key & cOutSelection);
		const bool ShowSelectedItem(Key & cOutSelection) const;

		bool ShowPath(KeyPath & cOutPath) const;

		bool ShowSelectionPosition(WindowPoint & cOutLocation) const;
		bool ShowSelectionPosition(WorldPoint & cOutLocation) const;

		bool KeyFront(Key & cInKey, H3DF::Type eInType = H3DF::Type::None);
		bool KeyFront(HC_KEY nInKey, H3DF::Type eInType = H3DF::Type::None);

		bool KeyPushBack(Key & cInKey, H3DF::Type eInType = H3DF::Type::None);
		bool KeyPushBack(HC_KEY nInKey, H3DF::Type eInType = H3DF::Type::None);
	};

	class API_3DF SelectionResultsIterator : public Object
	{
	public:
		SelectionResultsIterator();
		SelectionResultsIterator(SelectionResultsIterator const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::SelectionResultsIterator;
		H3DF::Type ObjectType() const { return staticType; };

		SelectionResultsIterator & operator=(SelectionResultsIterator const & cInThat);

		void Next();

		SelectionResultsIterator & operator++();
		SelectionResultsIterator & operator++(int nInVal);

		bool operator == (SelectionResultsIterator const & cInSearchResultsIterator);
		bool operator != (SelectionResultsIterator const & cInSearchResultsIterator);

		bool IsValid() const;

		void Reset();

		SelectionItem & GetItem() const;

		SelectionItem & operator * () const;
	};

	class API_3DF SelectionResults : public Object
	{
	public:
		// The default constructor creates an uninitialized SelectionItem object.  The Type() function will return Type::None.
		SelectionResults();
		SelectionResults(SelectionResults const & cInThat);
		~SelectionResults();

		SelectionResults & operator = (SelectionResults const & cInThat);

		SelectionResults(SelectionResults && cInThat) noexcept;
		SelectionResults & operator = (SelectionResults && cInThat) noexcept;

		bool operator==(SelectionResults const & cInThat) const;
		bool operator!=(SelectionResults const & cInThat) const;

		void Reset();
		void Reset() const;

		size_t GetCount() const;
		SelectionResultsIterator GetIterator() const;

		SelectionItem & Front();
		SelectionItem & Front() const;

		void PushFront(SelectionItem & cInItem);
		void PushBack(SelectionItem & cInItem);

		bool Erase(SelectionItem & cInItem);
		bool Erase(SelectionResults const & cInResults);

		void SetSize(size_t nInSize);

		bool Union(SelectionResults const & cInThat);

		void LeaveType(DWORD nType);
		void RemoveType(DWORD nType);

		bool Sort();

		bool IsExist(SelectionItem & cInItem);
	};

	class API_3DF SelectionControl : public Control
	{
	public:
		explicit SelectionControl(WindowKey const & cInWindow);
		SelectionControl(SelectionControl const & cInThat);
		~SelectionControl();

		static const H3DF::Type staticType = H3DF::Type::SelectionControl;
		H3DF::Type ObjectType() const { return staticType; };

		SelectionControl & operator=(SelectionControl const & cInThat);

		// Point in window space at which to perform the selection.
		size_t SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const;
		size_t SelectByPoint(Point const & cInLocation, SelectionResults & cOutResults) const;
		size_t SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const;
		size_t SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const;

	private:
		// Private default constructor to prevent instantiation without a window.
		SelectionControl();
	};
}