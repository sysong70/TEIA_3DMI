#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF 
{
	class API_3DF ColorInterpolationKit : public Kit 
	{
	public:
		ColorInterpolationKit();
		ColorInterpolationKit(ColorInterpolationKit const & cInKit);

		void Set(ColorInterpolationKit const & cInKit);
		ColorInterpolationKit const & operator = (ColorInterpolationKit const & cInKit);

		void Show(ColorInterpolationKit & cOutKit) const;

		bool Empty() const;

		bool Equals(ColorInterpolationKit const & cInKit) const;
		bool operator == (ColorInterpolationKit const & cInKit) const;
		bool operator != (ColorInterpolationKit const & cInKit) const;

		// Valid range : 0 <= fInNear <= fInFar <= 1
		ColorInterpolationKit & SetDepthRange(float fInNear, float fInFar);

		// 렌더링된 face를 카메라에서 밀어내는 하는 Z-buffer 단위수를 지정. 
		// face 위에 edge가 동일하게 있는 경우 edge를 face위에 놓이도록 해 스티칭을 줄이는데 사용할 수 있음. 음수인 경우 카메라로 다가오게 됨.
		ColorInterpolationKit & SetFaceDisplacement(int nInBuckets);

		ColorInterpolationKit & UnsetDepthRange();
		ColorInterpolationKit & UnsetFaceDisplacement();

		bool ShowDepthRange(float & fOutNear, float & fOutFar) const;
		bool ShowFaceDisplacement(int & nOutBuckets) const;
	};

	class API_3DF ColorInterpolationControl : public Control
	{
	public:
		ColorInterpolationControl(SegmentKey & cInSegmentKey);
		ColorInterpolationControl(ColorInterpolationControl const & cInThat);

		void Set(ColorInterpolationControl const & cInThat);
		ColorInterpolationControl & operator = (ColorInterpolationControl const & cInThat);

		ColorInterpolationControl & SetFaceColor(bool bInState);
		ColorInterpolationControl & SetEdgeColor(bool bInState);
		ColorInterpolationControl & SetVertexColor(bool bInState);

		ColorInterpolationControl & UnsetFaceColor();
		ColorInterpolationControl & UnsetEdgeColor();
		ColorInterpolationControl & UnsetVertexColor();
		ColorInterpolationControl & UnsetEverything();

		bool ShowFaceColor(bool & bOutState) const;
		bool ShowEdgeColor(bool & bOutState) const;
		bool ShowVertexColor(bool & bOutState) const;

	private:
		// Private default constructor to prevent instantiation without a segment.
		ColorInterpolationControl();
	};
}
