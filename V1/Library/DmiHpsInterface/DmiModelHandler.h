#pragma once

#include "ComponentTree/DmiHpsComponentTree.h"

class DmiHpsInterface;
class DmiHpsFileInterface;
class DmiHpsView;

class DmiModelHandler
{
public:
	DmiModelHandler(DmiHpsInterface * pcHpsInterface);
	~DmiModelHandler();

	bool CreateNewModel();

	bool DeleteModel();
	
	HPS::CADModel GetCADModel() const { return m_cCadModel; }
	void SetCadModel(HPS::CADModel const & cCadModel) { m_cCadModel = cCadModel; }
	void DeleteCadModel();

	HPS::Model & GetModel() { return m_cModel; }
	//HPS::CADModel & GetCADModel() { return m_cCadModel; }
	HPS::CameraKit & GetDefaultCamera() { return m_cDefaultCamera; }

	DmiHpsView * GetHpsView() { return m_pcHpsView; }
	void SetHpsView(DmiHpsView * pcHpsView) { m_pcHpsView = pcHpsView; }

	DmiHpsComponentTreePtr GetComponentTree() { return m_pcComponentTree; }
	void SetComponentTree(DmiHpsComponentTreePtr pcComponentTree) { m_pcComponentTree = pcComponentTree; }

	// == File 관련 함수 =============================================================================
	DmiHpsFileInterface * GetHpsFileInterface();
	bool FileImport(CString strFilePathName);

	bool ImportExchangeFile(CString strFilePathName, std::chrono::system_clock::time_point * pcTimes);
	bool RequestImportExchangeNotifierStatus();
	bool ImportExchangeLog();
	bool CompleteImportExchangeFile();

private:
	HPS::Model m_cModel;
	HPS::CADModel m_cCadModel;
	HPS::CameraKit m_cDefaultCamera;

	DmiHpsInterface * m_pcHpsInterface = nullptr;
	DmiHpsFileInterface * m_pcFileInterface = nullptr;

	DmiHpsView * m_pcHpsView = nullptr;

	DmiHpsComponentTreePtr m_pcComponentTree = nullptr;
};