#pragma once

#include "../3DF.h"
#include "../Math.h"
#include "../Impl/ControlImpl.h"

namespace H3DF
{
	class CameraKitImpl : public Impl
	{
	public:
		CameraKitImpl() { m_eType = H3DF::Type::CameraKit; }

		void Copy(CameraKitImpl * pcInThat)
		{
			m_cData.cUpVector = pcInThat->m_cData.cUpVector;
			m_cData.bUpVectorFlag = pcInThat->m_cData.bUpVectorFlag;
			m_cData.cPosition = pcInThat->m_cData.cPosition;
			m_cData.bPositionFlag = pcInThat->m_cData.bPositionFlag;
			m_cData.cTarget = pcInThat->m_cData.cTarget;
			m_cData.bTargetFlag = pcInThat->m_cData.bTargetFlag;
		}

		struct Data
		{
			Point cPosition; bool bPositionFlag = false;
			Point cTarget; bool bTargetFlag = false;
			Vector cUpVector; bool bUpVectorFlag = false;
			float fWidth = 0.0f; bool bWidthFlag = false;
			float fHeight = 0.0f; bool bHeightFlag = false;
			float fNearLimit = 0.0f; bool bNearLimitFlag = false;
			Camera::Projection eType = Camera::Projection::Default;  bool bTypeFlag = false;
			float fOblique_Y_Skew = 0.0f;
			float fOblique_X_Skew = 0.0f;
		};

		Data m_cData;
	};
}

namespace H3DF
{
	class CameraControlImpl : public ControlImpl
	{
	public:
		CameraControlImpl() { m_eType = H3DF::Type::CameraControl; }

		void Copy(CameraControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}
