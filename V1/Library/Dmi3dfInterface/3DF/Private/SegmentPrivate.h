#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Color.h"

#include "KeyPrivate.h"

namespace H3DF
{
	class BaseView;

	class SegmentKeyPrivate : public KeyPrivate
	{
	public:
		SegmentKeyPrivate() { m_eType = Type::SegmentKey; }
		~SegmentKeyPrivate() {}

		void Copy(SegmentKeyPrivate * pcInThat);

		//== Segment 관련 함수 ===========================================================================
		void LocalOpen();
		void LocalOpen() const;
		static void LocalOpen(SegmentKey & cSegmentKey);
		static void LocalOpen(SegmentKey const & cSegmentKey);

		void LocalClose();
		void LocalClose() const;
		static void LocalClose(SegmentKey & cSegmentKey);
		static void LocalClose(SegmentKey const & cSegmentKey);

		void Open();
		void Close();

		bool IsLocalOpen() const;
		bool IsForcedOpen() const;

		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

		void SetColor(CString strInGeometryName, RGBAColor cInColor);

		BaseView * GetBaseView() const;
		void SetBaseView(BaseView * pcInBaseView);

	private:
		BaseView * m_pcBaseView = nullptr;

		mutable bool m_bOpen = false;
		mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
	};
}