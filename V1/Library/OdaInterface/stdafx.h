#pragma once

#include "framework.h"
#include "Common_Define.h"
#include "OdaCommon.h"

#include <condition_variable>
#include <functional>
#include <memory>
#include <map>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "File.h"
#include "Json.h"
#include "WStr.h"

#include "Signal2d.h"
#include "../../UiMain/Cmd.Resource.h"

#ifdef ODAINTERFACE_EXPORTS
#define INTERFACE_API __declspec(dllexport)
#else
#define INTERFACE_API __declspec(dllimport)
#endif
