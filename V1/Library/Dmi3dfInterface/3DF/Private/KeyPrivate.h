#pragma once

#include "../3DF.h"
#include "../Object.h"

namespace H3DF
{
	class API_3DF KeyPrivate : public H3DF::PrivateImpl
	{
	public:
		KeyPrivate() { m_eType = Type::Key; }
		~KeyPrivate() {}

		HC_KEY m_nKey = INVALID_KEY;

		void Copy(KeyPrivate * pcInThat)
		{
			m_nKey = pcInThat->m_nKey;
			m_eType = pcInThat->Type();
		}

		// 부분 함수(Local Function)에서 사용하는 함수
		void LocalOpen();
		void LocalOpen() const;
		static void LocalOpen(Key & cSegmentKey);
		static void LocalOpen(Key const & cSegmentKey);

		void LocalClose();
		void LocalClose() const;
		static void LocalClose(Key & cSegmentKey);
		static void LocalClose(Key const & cSegmentKey);

		void ForcedOpen();
		void ForcedOpen() const;
		static void ForcedOpen(Key & cSegmentKey);
		static void ForcedOpen(Key const & cSegmentKey);

		void ForcedClose();
		void ForcedClose() const;
		static void ForcedClose(Key & cSegmentKey);
		static void ForcedClose(Key const & cSegmentKey);

		void Open();
		void Close();

		bool IsLocalOpen() const;
		bool IsForcedOpen() const;

		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

	protected:
		mutable bool m_bOpen = false;
		mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
	};
}