#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF 
{
	class API_3DF DrawingAttributeKit : public Kit 
	{
	public:
		DrawingAttributeKit();
		DrawingAttributeKit(DrawingAttributeKit const & cInKit);

		void Set(DrawingAttributeKit const & cInKit);
		DrawingAttributeKit const & operator = (DrawingAttributeKit const & cInKit);

		void Show(DrawingAttributeKit & cOutKit) const;

		bool Empty() const;

		bool Equals(DrawingAttributeKit const & cInKit) const;
		bool operator == (DrawingAttributeKit const & cInKit) const;
		bool operator != (DrawingAttributeKit const & cInKit) const;

		// Valid range : 0 <= fInNear <= fInFar <= 1
		DrawingAttributeKit & SetDepthRange(float fInNear, float fInFar);

		// 렌더링된 face를 카메라에서 밀어내는 하는 Z-buffer 단위수를 지정. 
		// face 위에 edge가 동일하게 있는 경우 edge를 face위에 놓이도록 해 스티칭을 줄이는데 사용할 수 있음. 음수인 경우 카메라로 다가오게 됨.
		DrawingAttributeKit & SetFaceDisplacement(bool bInState, int bInBuckets = 8);
		DrawingAttributeKit & SetFaceDisplacement(int bInBuckets);

		DrawingAttributeKit & UnsetDepthRange();
		DrawingAttributeKit & UnsetFaceDisplacement();
	};
}
