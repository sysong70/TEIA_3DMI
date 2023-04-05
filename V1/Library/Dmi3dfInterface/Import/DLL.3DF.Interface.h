#pragma once

#include "DLL.Helper.h"

#include "../../Dmi3dxInterface/DLL.h"

#include "Json.h"

OPEN_DLL_NAMESPACE

namespace TDF {

	class Interface
	{
	protected:
		Helper m_cDLL{ L"3DMI3dxInterface.dll" };

	public:
		decltype(_3DFImportFile) * _3DFImportFile = m_cDLL["_3DFImportFile"];
	};

}

CLOSE_DLL_NAMESPACE