#pragma once

#include <HDB.h>
#include <HBaseModel.h>
#include <HTools.h>
#include <HStream.h>
#include <HUtility.h>

#include "3DF.h"
#include "Segment.h"

class HIOConnector;

OPEN_3DF_NAMESPACE

class MultiSelectManager;

class Model : public HBaseModel
{
public:
	Model();
	virtual ~Model();

	SegmentKey GetSegmentKey() { return m_cSegmentKey; }

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

CLOSE_3DF_NAMESPACE