#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"
#include "Math.h"

namespace H3DF
{
	class API_3DF Condition
	{
	public:
		enum class Intrinsic : uint32_t
		{
			None,
			Extent,
			DrawPass,
			InnerPixelWidth,
			InnerPixelHeight,
			Selection,
			QuickMovesProbe,
		};

	private:
		Condition() {}
	};

	class API_3DF ConditionalExpression : public Object
	{
	public:
		ConditionalExpression();
		ConditionalExpression(CStringA strInCondition);
		ConditionalExpression(float fInNumber);
		ConditionalExpression(Condition::Intrinsic eInSpecial);
		ConditionalExpression(ConditionalExpression const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::ConditionalExpression;
		H3DF::Type ObjectType() const { return staticType; };

		ConditionalExpression & operator = (ConditionalExpression const & cInThat);

		bool ShowCondition(CStringA & strOutCondition) const;
		bool ShowNumber(float & fOutNumber) const;
		bool ShowIntrinsic(Condition::Intrinsic & eOutSpecial) const;

		bool Equals(ConditionalExpression const & cInThat) const;
		bool operator == (ConditionalExpression const & cInThat) const;
		bool operator != (ConditionalExpression const & cInThat) const;

		friend API_3DF ConditionalExpression OR(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2);
		friend API_3DF ConditionalExpression AND(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2);
	};

	API_3DF ConditionalExpression OR(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2);
	API_3DF ConditionalExpression AND(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2);

	class API_3DF ConditionControl : public Control
	{
	public:
		ConditionControl(SegmentKey & cInSegment);
		ConditionControl(ConditionControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::ConditionControl;
		H3DF::Type ObjectType() const { return staticType; };

		ConditionControl & operator = (ConditionControl const & cInThat);

		size_t GetCount() const;

		// Adds a condition to the collection of active conditions on this segment.
		ConditionControl & AddCondition(CStringA strInCondition);

		// Sets a condition as the only active condition on this segment, replacing any existing conditions.
		ConditionControl & SetCondition(CStringA strInCondition);

		// Removes a specified condition from the active conditions on this segment.
		ConditionControl & UnsetCondition(CStringA strInCondition);

		// Unsets all conditions on this segment.
		ConditionControl & UnsetEverything();

		// Queries the existence of one condition on this segment.
		bool ShowCondition(CStringA & strOutCondition) const;

		// Shows all conditions on this segment.
		bool ShowConditions(AStringArray & strOutConditions) const;
	};
}