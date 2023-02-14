#include "stdafx.h"
#include "ExOdaServices.h"
#include "path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

ExOdaServicesStatic TheOdaServices;

//**************************************************************************************************

OdString ExOdaServices::findFile(const OdString& pcFilename, OdDbBaseDatabase* pDb /*= NULL*/, OdDbBaseHostAppServices::FindFileHint hint /*= kDefault*/)
{
	RETURN("");
}

//--------------------------------------------------------------------------------------------------

//:WARNING - used at once!

OdString ExOdaServices::getFontMapFileName() const
{
	RETURN("");
}



OdString ExOdaServices::getSubstituteFont(const OdString& fontName, OdFontType fontType)
{
	RETURN("");
}
