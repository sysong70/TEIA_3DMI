#pragma once

#include "ExSystemServices.h"
#include "ExHostAppServices.h"

//----------------------------------------------------------------------------------------

class ExOdaServices
	: public ExSystemServices
	, public ExHostAppServices
{
protected:

	ODRX_USING_HEAP_OPERATORS(ExSystemServices);

public:

	virtual OdString findFile(const OdString& pcFilename, OdDbBaseDatabase* pDb = NULL, OdDbBaseHostAppServices::FindFileHint hint = kDefault);

public:	// OdDbHostAppServices

	OdString getFontMapFileName() const override;

	OdString getSubstituteFont(const OdString& fontName, OdFontType fontType) override;
};

typedef OdStaticRxObject<ExOdaServices> ExOdaServicesStatic;

//----------------------------------------------------------------------------------------

extern ExOdaServicesStatic TheOdaServices;
