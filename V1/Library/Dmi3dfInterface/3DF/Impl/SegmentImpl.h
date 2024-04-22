#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Color.h"

#include "KeyImpl.h"

namespace H3DF
{
	class BaseView;

	class API_3DF SegmentKeyImpl : public KeyImpl
	{
	public:
		SegmentKeyImpl() { m_eType = Type::SegmentKey; }
		~SegmentKeyImpl() {}

		void Copy(SegmentKeyImpl * pcInThat);

		//== Segment 관련 함수 ===========================================================================

		// 부분 함수(Local Function)에서 사용하는 함수
		void LocalOpen();
		void LocalOpen() const;
		static void LocalOpen(SegmentKey & cSegmentKey);
		static void LocalOpen(SegmentKey const & cSegmentKey);

		void LocalClose();
		void LocalClose() const;
		static void LocalClose(SegmentKey & cSegmentKey);
		static void LocalClose(SegmentKey const & cSegmentKey);

		void ForcedOpen();
		void ForcedOpen() const;
		static void ForcedOpen(SegmentKey & cSegmentKey);
		static void ForcedOpen(SegmentKey const & cSegmentKey);

		void ForcedClose();
		void ForcedClose() const;
		static void ForcedClose(SegmentKey & cSegmentKey);
		static void ForcedClose(SegmentKey const & cSegmentKey);

		void Open();
		void Close();

		bool IsLocalOpen() const;
		bool IsForcedOpen() const;

		void SetColor(CStringA strInGeometryName, RGBAColor cInColor);

		BaseView * GetBaseView() const;
		void SetBaseView(BaseView * pcInBaseView);

		BoundingKit * m_pcBoundingKit = nullptr;

	private:
		BaseView * m_pcBaseView = nullptr;
	
		mutable bool m_bOpen = false;
		mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
	};
}