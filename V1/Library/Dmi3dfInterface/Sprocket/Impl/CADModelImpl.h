#pragma once

#include "../../3DF/3DF.h"

#include "ComponentImpl.h"

#include <A3DSDKIncludes.h>

#include <atlcoll.h>

namespace H3DF
{
	class CADModelImpl : public ComponentImpl
	{
	public:
		CADModelImpl();
		~CADModelImpl();

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<CADModelImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const CADModelImpl * pcInThat);

		Component & ModelComponent();

		Component * m_pcModels = nullptr;
		Component * m_pcMeasurements = nullptr;
		Component * m_pcMarkups = nullptr;

		DWORD m_nProductOccurrenceIndex = 1;
		DWORD m_nSolidIndex = 1;
		DWORD m_nSurfaceIndex = 1;
		DWORD m_nGroupIndex = 1;
		DWORD m_nPointSetIndex = 1;
		DWORD m_nCurveIndex = 1;
		DWORD m_nPointIndex = 1;

		void MapSetAt(HC_KEY nInKey, Component * pcInComponent);
		CAtlMap<HC_KEY, Component *> * m_pmComponentMap;
		CAtlMap<DWORD_PTR, A3DEntity *> * m_pmEntityMap;
	};

	class CADModel;

	namespace CADModelUtility
	{
		API_3DF void MapSetAt(CADModel * pcInCadModel, HC_KEY nInKey, Component * pcInComponent);
		API_3DF CString TypeName(CADModel * pcInCadModel, const Component & cInComponent);
	}
}
