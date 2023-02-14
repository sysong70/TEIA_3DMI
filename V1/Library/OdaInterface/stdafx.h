#pragma once

#include "framework.h"
#include "Common_Define.h"
#include "OdaCommon.h"

#ifdef ODAINTERFACE_EXPORTS
#define INTERFACE_API __declspec(dllexport)
#else
#define INTERFACE_API __declspec(dllimport)
#endif
