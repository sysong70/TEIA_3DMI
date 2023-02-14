#include "stdafx.h"

#define STL_USING_IOSTREAM
#define STL_USING_VECTOR
#include "OdaSTL.h"

#include "OdaCommon.h"
#include "OdToolKit.h"
#include "OdString.h"
#include "ExHostAppServices.h"
#include "Gs/Gs.h"
#include "DbAudit.h"
#include "Gi/TtfDescriptor.h"
#include "OdStreamBuf.h"
#include "RxDictionary.h"
#include "OdFontServices.h"
#include "OdCharMapper.h"

#define  STD(a)  std:: a

//--------------------------------------------------------------------------------------------------

ExHostAppServices::ExHostAppServices()
	: m_disableOutput(false)
{
}



OdHatchPatternManager* ExHostAppServices::patternManager()
{
	if (m_patternManager.isNull()) {
		// dna: use odrxSafeCreateObject() for no linking to TD_DbFull
		m_patternManager = odrxSafeCreateObject<OdHatchPatternManager>("OdHatchPatternManager", OdDbEntitiesAppName);
		m_patternManager->setApplicationService(this);
	}

	return m_patternManager.get();
}



OdDbHostAppProgressMeter* ExHostAppServices::newProgressMeter()
{
	return this;
}



void ExHostAppServices::releaseProgressMeter(OdDbHostAppProgressMeter* /*pMeter*/)
{
}



OdDbDatabasePtr ExHostAppServices::readFile(const OdString& fileName, bool bAllowCPConversion, bool bPartial, Oda::FileShareMode shmode, const OdPassword& password)
{
	OdDbDatabasePtr pRes = OdDbHostAppServices2::readFile(fileName, bAllowCPConversion, bPartial, shmode, password);

	return pRes;
}



void ExHostAppServices::start(const OdString& displayString)
{
	if (m_disableOutput == false) {
		odPrintConsoleString(OD_T("%ls------- Started %ls\n"), m_Prefix.c_str(), displayString.c_str());
	}
}



void ExHostAppServices::stop()
{
	if (m_disableOutput == false) {
		odPrintConsoleString(OD_T("%ls------- Stopped\n"), m_Prefix.c_str());
	}
}



void ExHostAppServices::meterProgress()
{
	m_MeterCurrent++;
	if (m_disableOutput == false) {
		double  f1, f2;

		f1 = (double)m_MeterCurrent / m_MeterLimit * 100;
		f2 = (double)m_MeterOld / m_MeterLimit * 100;
		if ((f1 - f2) > 0.7) {
			odPrintConsoleString(OD_T("%lsProgress: %2.2lf%%\n"), m_Prefix.c_str(), f1);
			m_MeterOld = m_MeterCurrent;
		}
	}
}



void ExHostAppServices::setLimit(int max)
{
	m_MeterLimit = max;
	m_MeterCurrent = 0;
	m_MeterOld = 0;

	if (!m_disableOutput) {
		odPrintConsoleString(OD_T("%lsMeter Limit: %d\n"), m_Prefix.c_str(), max);
	}
}
