#pragma once

#include "TD_PackPush.h"

#include "DbHostAppServices.h"
#include "HatchPatternManager.h"
#include "StaticRxObject.h"
#include "DbDatabaseReactor.h"
#include "Gi/TtfDescriptor.h"

#define STL_USING_MAP
#include "OdaSTL.h"

#include "ExPrintConsole.h"

//--------------------------------------------------------------------------------------------------

typedef OdArray<OdTtfDescriptor> mapTrueTypeFont;

/*
	This class implements platform-dependent operations and progress metering.
	<group ExServices_Classes>
	Library: Source code provided.
*/
class ExHostAppServices
	: public OdDbHostAppServices2
	, public OdDbHostAppProgressMeter
{
	OdString m_Prefix;
	long m_MeterLimit;
	long m_MeterCurrent;
	long m_MeterOld;
	bool m_disableOutput;
	OdHatchPatternManagerPtr m_patternManager;

public:

	ExHostAppServices();

	OdDbHostAppProgressMeter* newProgressMeter();

	void releaseProgressMeter(OdDbHostAppProgressMeter* pProgressMeter);

	void warning(const char*, const OdString&) {}

	TD_USING(OdDbHostAppServices2::warning);

	// OdDbHostAppProgressMeter functions

	void start(const OdString& displayString = OdString::kEmpty);

	void stop();

	void meterProgress();

	void setLimit(int max);
	/*
		Controls display of this ProgressMeter.

		disable [in] Disables this ProgressMeter.
	*/
	void disableOutput(bool disable)
	{
		m_disableOutput = disable;
	}
	/*
		Sets the prefix for this ProgressMeter.

		prefix [in] Prefix for this ProgressMeter.
	*/
	void setPrefix(const OdString& prefix)
	{
		m_Prefix = prefix;
	}

	OdHatchPatternManager* patternManager();

	OdDbDatabasePtr readFile(const OdString& filename, bool allowCPConversion = false, bool partialLoad = false, Oda::FileShareMode shareMode = Oda::kShareDenyNo, const OdPassword& password = OdPassword());

	TD_USING(OdDbHostAppServices2::readFile);
};

#include "TD_PackPop.h"
