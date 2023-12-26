#pragma once

#include "../3DF/3DF.h"

#include "../3DF/ApplicationWindowOption.h"

#include <HDB.h>
#include <HBaseModel.h>
#include <HTools.h>
#include <HStream.h>
#include <HUtility.h>
#include <HIOManager.h>

#include "../3DF.View.h"

namespace H3DF
{
	class MultiSelectManager;

	class ModelImpl : public HBaseModel, public Impl
	{
	public:
		ModelImpl();
		virtual ~ModelImpl();

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		void SetBRepGeometry(bool brep) override;

		H3DF::ModelHandedness GetModelHandedness() { return m_eModelHandedness; }

		void UpdateModelHandedness();

	private:
		SegmentKey m_cSegmentKey;

		ModelHandedness m_eModelHandedness;

		MultiSelectManager * m_pcMultiSelectManager;

		BREP_Topology * m_pcTopologyManager;
		HIOConnector * m_pPMIConnector;
		HIOConnector * m_pcConnector;

		void * m_pcPRCAsmModelFile;
		void (*m_pcPRCDeleteModelCallback) (void *& pPRCAsmModelFile);
	};
}