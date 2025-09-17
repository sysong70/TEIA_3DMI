#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Color.h"

#include "KeyImpl.h"

namespace H3DF
{
	class BaseView;

	class SegmentKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SegmentKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SegmentKeyImpl * pcInThat);

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

		WindowKey * GetWindow() const;
		void SetWindow(WindowKey * pcInWindow);

		static bool FindUp(SegmentKey & cInSegment, H3DF::Type eInType, SegmentKey & cOutFindSegment);

		std::unique_ptr<BoundingKit> m_pcBoundingKit;

	private:
		WindowKey * m_pcWindowKey = nullptr;
	
		mutable bool m_bOpen = false;
		mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
	};

	namespace SegmentKeyUtility
	{
		void API_3DF LocalOpen(SegmentKey & cSegmentKey);
		void API_3DF LocalOpen(SegmentKey const & cSegmentKey);

		void API_3DF LocalClose(SegmentKey & cSegmentKey);
		void API_3DF LocalClose(SegmentKey const & cSegmentKey);

		void API_3DF ForcedOpen(SegmentKey & cSegmentKey);
		void API_3DF ForcedOpen(SegmentKey const & cSegmentKey);

		void API_3DF ForcedClose(SegmentKey & cSegmentKey);
		void API_3DF ForcedClose(SegmentKey const & cSegmentKey);
	}
}