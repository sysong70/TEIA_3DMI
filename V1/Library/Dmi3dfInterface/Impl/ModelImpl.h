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

		void Init() override;

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		void SetBRepGeometry(bool brep) override;

		H3DF::ModelHandedness GetModelHandedness() { return m_eModelHandedness; }

		void UpdateModelHandedness();

		SegmentKey m_cInclude;

		SegmentKey m_cModels;
		SegmentKey m_cMeasurements;
		SegmentKey m_cMarkups;

		SegmentKey m_cIncludeSegment;
		SegmentKey m_cIncludeModel;
		SegmentKey m_cIncludeStyles;

		SegmentKey & ShowStyleSegment() { return m_cShowStyle; }
		SegmentKey & ShowVertexStyleSegment() { return m_cShowVertexStyle; }

		SegmentKey & NoShowStyleSegment() { return m_cNoShowStyle; }
		SegmentKey & NoShowVertexStyleSegment() { return m_cNoShowVertexStyle; }

		SegmentKey & ShowOnlyStyleSegment() { return m_cShowOnlyStyle; }
		SegmentKey & ShowOnlyVertexStyleSegment() { return m_cShowOnlyVertexStyle; }

	private:
		SegmentKey m_cSegmentKey;

		SegmentKey m_cShowStyle;
		SegmentKey m_cShowVertexStyle;

		SegmentKey m_cNoShowStyle;
		SegmentKey m_cNoShowVertexStyle;

		SegmentKey m_cShowOnlyStyle;
		SegmentKey m_cShowOnlyVertexStyle;

		ModelHandedness m_eModelHandedness;

		MultiSelectManager * m_pcMultiSelectManager;

		BREP_Topology * m_pcTopologyManager;
		HIOConnector * m_pPMIConnector;
		HIOConnector * m_pcConnector;

		void * m_pcPRCAsmModelFile;
		void (*m_pcPRCDeleteModelCallback) (void *& pPRCAsmModelFile);
	};
}