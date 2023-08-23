#pragma once

#include "../Dmi3dfInterface/3DF.Signal.Interface.h"
#include "../Dmi3dxInterface/3DXSignal.Interface.h"
#include "../DmiC3dInterface/DmiC3dInterface.h"

#include "Json.h"

#include <chrono>

#include "Session.h"

namespace SESSION
{
	using SendSignalFunc = void (*)(const wchar_t *);
	using AssignSendSignalFunc = void (*)(SendSignalFunc);

	class SignalAnalyzer
	{
	public:
		SignalAnalyzer();

		void ExecuteSignal(const wchar_t * pchBuffer);
		void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

		SendSignalFunc GetSendSignalFunc() { return m_pcSendSignalFunc; }

	private:
		H3DF::Interface m_3DF_Interface;

		//Dmi3dxInterface m_c3dxInterface;
		DmiC3dInterface m_C3D_Interface;

		SendSignalFunc m_pcSendSignalFunc = nullptr;

	protected:
		std::vector<std::string> m_vstrHpsErrMsgVector;
	};

};

extern SESSION::SignalAnalyzer theSignalAnalyzer;