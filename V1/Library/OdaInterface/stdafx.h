#pragma once

#include "framework.h"
#include "Common_Define.h"
#include "OdaCommon.h"

#include <map>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Signal.h"
#include "File.h"
#include "Json.h"
#include "WStr.h"

#include "..\..\UiMain\Command.Resource.h"

#ifdef ODAINTERFACE_EXPORTS
#define INTERFACE_API __declspec(dllexport)
#else
#define INTERFACE_API __declspec(dllimport)
#endif
