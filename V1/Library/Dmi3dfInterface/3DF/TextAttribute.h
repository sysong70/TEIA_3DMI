#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class API_3DF TextAttributeControl : public Control 
	{
	public:
		explicit TextAttributeControl(SegmentKey & cInSegment);
		TextAttributeControl(TextAttributeControl const & cInThat);

		TextAttributeControl(TextAttributeControl && cInThat) noexcept;
		TextAttributeControl & operator = (TextAttributeControl && cInThat) noexcept;

		~TextAttributeControl();

		TextAttributeControl & operator = (TextAttributeControl const & cInThat);

		TextAttributeControl & SetBackground(bool bInState, CStringA strInName);
		TextAttributeControl & SetBackground(CStringA strInName);
		TextAttributeControl & SetBackground(bool bInState);
	};

}