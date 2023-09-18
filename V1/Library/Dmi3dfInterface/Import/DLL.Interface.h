#pragma once

#include "DLL.Helper.h"

#include "../../Dmi3dxInterface/DLL.h"

#include "Json.h"

namespace DLL
{
	namespace H3DF {

		class Interface
		{
		protected:
			Helper m_cDLL{ L"3DMI3dxInterface.dll" };

		public:
			decltype(TDFImportFile) * TDFImportFile = m_cDLL["TDFImportFile"];
		};

	}
}