#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class API_3DF PerformanceKit : public Kit
	{
	public:
		PerformanceKit();
		PerformanceKit(PerformanceKit const & cInThat);

		void Set(PerformanceKit const & cInThat);
		PerformanceKit const & operator = (PerformanceKit const & cInThat);

		PerformanceKit & SetDisplayLists(Performance::DisplayLists eInDisplayList = Performance::DisplayLists::Segment);
		PerformanceKit & SetStaticModel(Performance::StaticModel eInModelType);

		PerformanceKit & UnsetDisplayLists();
		PerformanceKit & UnsetStaticModel();

		bool ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const;
		bool ShowStaticModel(Performance::StaticModel & eOutModelType) const;
	};


	class API_3DF PerformanceControl : public Control
	{
	public:
		explicit PerformanceControl(SegmentKey & cInSegmentKey);
		PerformanceControl(PerformanceControl const & cInThat);

		void Set(PerformanceControl const & cInThat);
		PerformanceControl & operator = (PerformanceControl const & cInThat);

		PerformanceControl & SetDisplayLists(Performance::DisplayLists eInDisplayList = Performance::DisplayLists::Segment);
		PerformanceControl & SetStaticModel(Performance::StaticModel eInModelType);

		PerformanceControl & UnsetDisplayLists();
		PerformanceControl & UnsetStaticModel();

		bool ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const;
		bool ShowStaticModel(Performance::StaticModel & eOutModelType) const;

	private:
		// Private default constructor to prevent instantiation without a segment.
		PerformanceControl();
	};
}
