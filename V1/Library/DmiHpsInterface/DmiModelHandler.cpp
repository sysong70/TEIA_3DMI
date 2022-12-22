#include "stdafx.h"

#include "DmiModelHandler.h"

#include "DmiHpsFileInterface.h"

// DmiHpsVisualize::CreateNewModelHandler(DWORD_PTR nId)에서 선언됨
DmiModelHandler::DmiModelHandler(DmiHpsInterface * pcHpsInterface) :
	m_pcHpsInterface(pcHpsInterface)
{
	//m_pcHpsInterface = pcHpsInterface;
}

DmiModelHandler::~DmiModelHandler()
{

}

bool DmiModelHandler::CreateNewModel()
{
	// Delete our model if we have one already
	if(m_cModel.Type() != HPS::Type::None) {
		m_cModel.Delete();
	}

	// Delete old CADModel
	m_cCadModel.Delete();

	// Create a new model for our view to attach to
	m_cModel = HPS::Factory::CreateModel();

	// Lose old default camera
	m_cDefaultCamera.Reset();

	// #HPS-Option: Static model option
	//m_cModel.GetSegmentKey().GetPerformanceControl().SetStaticModel(HPS::Performance::StaticModel::Attribute);
	m_cModel.GetSegmentKey().GetPerformanceControl().SetStaticModel(HPS::Performance::StaticModel::Attribute);
		//SetStaticConditions(HPS::Performance::StaticConditions::Single);
		//SetDisplayLists(HPS::Performance::DisplayLists::Segment)
 		//SetDisplayLists(HPS::Performance::DisplayLists::Geometry);

/*
	m_bUseFramerate = FALSE;
	m_csFramerateMode = "Fixed Framerate";
	m_bCullingThreshold = TRUE;
	m_CullingThreshold = 10;
	m_csDisplayList = "Segment";
	m_bStaticModel = TRUE;
	m_bLMV = TRUE;
*/

	return true;
}

bool DmiModelHandler::DeleteModel()
{
	m_cModel.Delete();
	//m_cCadModel.Delete();

	return true;
}

void DmiModelHandler::DeleteCadModel()
{
	m_cCadModel.Delete();
}

// == File 관련 함수 ================================================================================
DmiHpsFileInterface * DmiModelHandler::GetHpsFileInterface() 
{ 
	if(nullptr == m_pcFileInterface) {
		assert(false);
	}

	return m_pcFileInterface; 
}

bool DmiModelHandler::FileImport(CString strFilePathName)
{
	if(nullptr == m_pcFileInterface) {
		m_pcFileInterface = new DmiHpsFileInterface(m_pcHpsInterface);
		CHECK_POINTER(m_pcFileInterface);
	}

	return m_pcFileInterface->FileImport(strFilePathName, this);
}

bool DmiModelHandler::ImportExchangeFile(CString strFilePathName, std::chrono::system_clock::time_point * pcTimes)
{
	if(nullptr == m_pcFileInterface) {
		m_pcFileInterface = new DmiHpsFileInterface(m_pcHpsInterface);
		CHECK_POINTER(m_pcFileInterface);
	}

	return m_pcFileInterface->ImportExchangeFile(strFilePathName, this, pcTimes);
}

bool DmiModelHandler::RequestImportExchangeNotifierStatus()
{
	return m_pcFileInterface->RequestImportExchangeNotifierStatus();
}

bool DmiModelHandler::ImportExchangeLog()
{
	return m_pcFileInterface->ImportExchangeLog();
}

bool DmiModelHandler::CompleteImportExchangeFile()
{
	return m_pcFileInterface->CompleteImportExchangeFile(this);
}