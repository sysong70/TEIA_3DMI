#pragma once

#include "StaticRxObject.h"
#include "Gi/GiDrawable.h"
#include "Gi/GiDrawableImpl.h"
#include "Gs/Gs.h"
#include "Gs/GsSelectionReactor.h"
#include "Si/SiSpatialIndex.h"

//--------------------------------------------------------------------------------------------------

namespace Tracker
{
	class OSnapManager
		: public OdGiDrawableImpl<>
		, public OdGsSelectionReactor
	{
	protected:

		OSnapManager() {}

	private: // OdGiDrawable

		OdUInt32 subSetAttributes(OdGiDrawableTraits* pDt) const { return 0; }

		void subViewportDraw(OdGiViewportDraw* pWd) const {}

		bool subWorldDraw(OdGiWorldDraw* pWd) const { return false; }

	private: // OdGsSelectionReactor

		bool selected(const OdGiDrawableDesc& pDrawableDesc) { return false; }

		OdUInt32 selected(const OdGiPathNode& pathNode, const OdGiViewport& viewInfo) { return 0; }

	public:
	};

	typedef OdStaticRxObject<OSnapManager> OSnapManagerStatic;
}
