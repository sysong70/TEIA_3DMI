#pragma once

// CString용 Header
#include <atlstr.h>

#include "../../Library/Dmi3dfInterface/3DF/3DF.h"
#include "../Signal/Signal.h"

#define OPEN_DLL_NAMESPACE namespace DLL {
#define CLOSE_DLL_NAMESPACE }

#define USING_DLL_NAMESPACE using namespace DLL;

#ifdef __cplusplus
extern "C" {
#endif  / * __cplusplus * /

	_declspec (dllexport) bool _3DFImportFile(CString pchFilePathName, _3DF::SegmentKey & cModelSegmentKey, Signal::Delivery & cInDelivery, CString & strErrorMessage);

#ifdef __cplusplus
}
#endif