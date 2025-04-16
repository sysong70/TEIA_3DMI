#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Segment.h"
#include "../../3DF/Window.h"

#include <HBaseView.h>
#include <HUtility.h>

#include <Json.h>

namespace H3DF
{
	class ViewImpl : public Impl
	{
	public:
		virtual ~ViewImpl();
		void Copy(const ViewImpl * pcInThat);

	public:
		SegmentKey GetSegmentKey() { return m_cKey; }
		SegmentKey GetModelKey() { return m_cModelKey; }

		Model & GetAttachedModel() {return *m_pcModel;}
		
		H3DF::WindowKey GetWindowKey();

		H3DF::BaseView * GetBaseView();

		//== Visual Effects 관련 함수 ================================================================
	public:
		bool GetSimpleShadow();
		void SetSimpleShadow(bool bFlag); 

		bool GetSimpleReflection();
		void SetSimpleReflection(bool bFlag);

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool KeyboardInput(Json::Object & cInObject);

	protected:
		DWORD MouseMapFlags(DWORD nState);

	protected:
		static bool GetKeyState(unsigned int key, int & flags);

	public:
		H3DF::WindowKey m_cWindow;
		CStringA m_strName;
		H3DF::Rendering::Mode m_eRenderingMode = H3DF::Rendering::Mode::Default;

		SegmentKey m_cKey;

		H3DF::Model * m_pcModel = nullptr;
		SegmentKey m_cModelKey;

		bool m_bSimpleShadowFlag = false;
		bool m_bSimpleReflection = false;
	};
}