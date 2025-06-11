#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class API_3DF Edge
	{
	public:

		// Edge Size Units
		enum class SizeUnits : uint32_t
		{
			ScaleFactor,			// A unitless linear scaling factor.  A value of 2.0 will cause edges to be rendered twice as thick.  
									// A value of 1.0 will result in a visually pleasing device-dependent edge weight.  
									// A value of 0 will result in the minimal weight edge supported by the device.
			ObjectSpace,			// Object space units ignoring any scaling components in modelling matrices. (oru)
			SubscreenRelative,		// Fraction of the height of the outermost window. (sru)
			WindowRelative,			// Fraction of the height of the local window. (wru)
			WorldSpace,				// Object space units including any scaling components in modelling matrices and cameras. (wsu)
			Points,					// Points units typically used for text size. 1 point corresponds to 1/72 inch. (points)
			Pixels,					// Number of pixels. (pixels)
		};

	private:
		Edge() = default;
	};

	class API_3DF EdgeAttributeKit : public Kit
	{
	public:
		EdgeAttributeKit();
		EdgeAttributeKit(EdgeAttributeKit const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::EdgeAttributeKit;
		H3DF::Type ObjectType() const { return staticType; };

		EdgeAttributeKit const & operator = (EdgeAttributeKit const & cInThat);

		void Show(EdgeAttributeKit & cOutKit) const;

		bool Empty() const;

		bool operator == (EdgeAttributeKit const & cInThat) const;
		bool operator != (EdgeAttributeKit const & cInThat) const;

		EdgeAttributeKit & SetPattern(CString strInPatternName);
		EdgeAttributeKit & SetWeight(float fInWeight, Edge::SizeUnits eInUnits = Edge::SizeUnits::ScaleFactor);
		EdgeAttributeKit & SetHardAngle(float fInAngle);

		EdgeAttributeKit & UnsetPattern();
		EdgeAttributeKit & UnsetWeight();
		EdgeAttributeKit & UnsetHardAngle();
		EdgeAttributeKit & UnsetEverything();

		bool ShowPattern(CString & strOutPatternName) const;
		bool ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const;
		bool ShowHardAngle(float & fOutAngle) const;
	};


	class API_3DF EdgeAttributeControl : public Control
	{
	public:
		EdgeAttributeControl(SegmentKey & cInSegment);
		EdgeAttributeControl(EdgeAttributeControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::EdgeAttributeControl;
		H3DF::Type ObjectType() const { return staticType; };

		EdgeAttributeControl & operator = (EdgeAttributeControl const & cInThat);

		EdgeAttributeControl & SetPattern(CString strInPatternName);
		EdgeAttributeControl & SetWeight(float fInWeight, Edge::SizeUnits eInUnits = Edge::SizeUnits::ScaleFactor);
		EdgeAttributeControl & SetHardAngle(float fInAngle);

		EdgeAttributeControl & UnsetPattern();
		EdgeAttributeControl & UnsetWeight();
		EdgeAttributeControl & UnsetHardAngle();
		EdgeAttributeControl & UnsetEverything();

		bool ShowPattern(CString & strOutPatternName) const;
		bool ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const;
		bool ShowHardAngle(float & fOutAngle) const;

	private:
		// Private default constructor to prevent instantiation without a segment.
		EdgeAttributeControl();
	};
}