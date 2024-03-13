#include "StdAfx.h"

#include "DLL.h"

#include "3DX.3DFImport.h"

#include <3DF/Segment.h>

bool TDFImportFile(CString pchFilePathName, H3DF::SegmentKey & cModelSegmentKey, H3DF::CADModel & cInCADModel, Signal::Delivery & cInDelivery, CString & strErrorMessage)
{
	H3DX::TdfImport cImportFile(nullptr);
	if(false == cImportFile.FileImport(pchFilePathName, cModelSegmentKey, cInCADModel, cInDelivery, strErrorMessage)) {
		return false;
	}

	return true;
}
