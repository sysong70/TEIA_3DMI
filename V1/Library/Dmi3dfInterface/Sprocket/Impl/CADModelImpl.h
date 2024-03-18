#pragma once

#include "ComponentImpl.h"

#include <atlcoll.h>

namespace H3DF
{
	class API_3DF CADModelImpl : public ComponentImpl
	{
	public:
		CADModelImpl();
		~CADModelImpl();
		void Copy(CADModelImpl * pcInThat);

		CString TypeName(const Component & cInComponent);

		Component & ModelComponent();

		Component * m_pcModels = nullptr;
		Component * m_pcMeasurements = nullptr;
		Component * m_pcMarkups = nullptr;

		DWORD m_nSolidIndex = 1;
		DWORD m_nSurfaceIndex = 1;
		DWORD m_nGroupIndex = 1;
		DWORD m_nPointSetIndex = 1;
		DWORD m_nCurveIndex = 1;
		DWORD m_nPointIndex = 1;

		void MapSetAt(HC_KEY nInKey, Component * pcInComponent);
		CAtlMap<HC_KEY, Component *> * m_pmComponentMap = nullptr;
	};
}