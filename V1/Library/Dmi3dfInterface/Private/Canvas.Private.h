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

	class Model : public HBaseModel, public PrivateImpl
	{
	public:
		Model();
		virtual ~Model();

		SegmentKey GetSegmentKey();

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

	class CanvasPrivate : public PrivateImpl
	{
	public:
		CanvasPrivate();
		virtual ~CanvasPrivate();

		void Copy(const CanvasPrivate * pcInThat);

		// 초기 입력 변수값
		H3DF::WindowHandle m_nInWindowHandle = 0;
		char * m_pchName = NULL;
		H3DF::ApplicationWindowOptionsKit m_cApplicationWindowOptionsKit;

		std::vector<H3DF::View * > m_vpcViewArray;

		H3DF::Model * m_pcModel = nullptr;
	};
}