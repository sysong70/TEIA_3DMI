#include "StdAfx.h"

#include "DLL.h"

#include "3DX.3DFImport.h"

#include <3DF/3DF.Segment.h>

bool _3DFImportFile(CString pchFilePathName, _3DF::SegmentKey & cModelSegmentKey, Signal::Delivery & cInDelivery, CString & strErrorMessage)
{
	_3DX::_3DfImport cImportFile(nullptr);
	if(false == cImportFile.FileImport(pchFilePathName, cModelSegmentKey, cInDelivery, strErrorMessage)) {
		return false;
	}

	return true;
}
