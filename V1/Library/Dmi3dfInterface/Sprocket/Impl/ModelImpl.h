#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Portfolio.h"
#include "../../3DF/ApplicationWindowOption.h"

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

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ModelImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ModelImpl * pcInThat) {
			m_cSegmentKey = pcInThat->m_cSegmentKey;
			m_cPortfolio = pcInThat->m_cPortfolio;
			m_cShowStyle = pcInThat->m_cShowStyle;
			m_cShowWireFrameStyle = pcInThat->m_cShowWireFrameStyle;
			m_cShowVertexStyle = pcInThat->m_cShowVertexStyle;
			m_cNoShowStyle = pcInThat->m_cNoShowStyle;
			m_cNoShowWireFrameStyle = pcInThat->m_cNoShowWireFrameStyle;
			m_cNoShowVertexStyle = pcInThat->m_cNoShowVertexStyle;
			m_eModelHandedness = pcInThat->m_eModelHandedness;
			m_pcMultiSelectManager = pcInThat->m_pcMultiSelectManager;
			m_pcTopologyManager = pcInThat->m_pcTopologyManager;
			m_pPMIConnector = pcInThat->m_pPMIConnector;
			m_pcConnector = pcInThat->m_pcConnector;
			m_pcPRCAsmModelFile = pcInThat->m_pcPRCAsmModelFile;
			m_pcPRCDeleteModelCallback = pcInThat->m_pcPRCDeleteModelCallback;
		}

		void Init() override;

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		PortfolioKey GetPortfolioKey();
		PortfolioKey const GetPortfolioKey() const;

		void SetBRepGeometry(bool brep) override;

		H3DF::ModelHandedness GetModelHandedness() { return m_eModelHandedness; }

		void UpdateModelHandedness();

		SegmentKey m_cInclude;

		SegmentKey m_cModelsRoot;
		SegmentKey m_cMeasurementsRoot;
		SegmentKey m_cMarkupsRoot;

		SegmentKey m_cIncludeSegment;
		SegmentKey m_cIncludeModel;
		SegmentKey m_cIncludeStyles;

		SegmentKey & ShowStyleSegment() { return m_cShowStyle; }
		SegmentKey & ShowWireFrameStyleSegment() { return m_cShowWireFrameStyle; }
		SegmentKey & ShowVertexStyleSegment() { return m_cShowVertexStyle; }

		SegmentKey & NoShowStyleSegment() { return m_cNoShowStyle; }
		SegmentKey & NoShowWireFrameStyleSegment() { return m_cNoShowWireFrameStyle; }
		SegmentKey & NoShowVertexStyleSegment() { return m_cNoShowVertexStyle; }

	private:
		SegmentKey m_cSegmentKey;

		PortfolioKey m_cPortfolio;

		SegmentKey m_cShowStyle;
		SegmentKey m_cShowWireFrameStyle;
		SegmentKey m_cShowVertexStyle;

		SegmentKey m_cNoShowStyle;
		SegmentKey m_cNoShowWireFrameStyle;
		SegmentKey m_cNoShowVertexStyle;

		ModelHandedness m_eModelHandedness;

		MultiSelectManager * m_pcMultiSelectManager;

		BREP_Topology * m_pcTopologyManager;
		HIOConnector * m_pPMIConnector;
		HIOConnector * m_pcConnector;

		void * m_pcPRCAsmModelFile;
		void (*m_pcPRCDeleteModelCallback) (void *& pPRCAsmModelFile);
	};
}