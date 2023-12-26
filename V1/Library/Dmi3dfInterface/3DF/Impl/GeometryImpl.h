#pragma once

#include "..\3DF.h"
#include "..\Key.h"
#include "..\Math.h"

#include "KeyImpl.h"

namespace H3DF
{
	class API_3DF GeometryKeyImpl : public KeyImpl
	{
	public:
		GeometryKeyImpl() { m_eType = Type::GeometryKey; }
		virtual ~GeometryKeyImpl() {}

		void Copy(GeometryKeyImpl * pcInThat);

		// 부분 함수(Local Function)에서 사용하는 함수
		void LocalOpen();
		void LocalOpen() const;
		void LocalClose();
		void LocalClose() const;

		static void LocalOpen(GeometryKey & cGeometryKey);
		static void LocalOpen(GeometryKey const & cGeometryKey);
		static void LocalClose(GeometryKey & cGeometryKey);
		static void LocalClose(GeometryKey const & cGeometryKey);

		void ForcedOpen();
		void ForcedOpen() const;
		void ForcedClose();
		void ForcedClose() const;

		static void ForcedOpen(GeometryKey & cGeometryKey);
		static void ForcedOpen(GeometryKey const & cGeometryKey);
		static void ForcedClose(GeometryKey & cGeometryKey);
		static void ForcedClose(GeometryKey const & cGeometryKey);

		void Open();
		void Close();

		bool IsLocalOpen() const;
		bool IsForcedOpen() const;

	protected:
		mutable bool m_bOpen = false;
		mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
	};
}
