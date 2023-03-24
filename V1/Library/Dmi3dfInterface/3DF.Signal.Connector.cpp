#include "StdAfx.h"

#include "3DF.Signal.Connector.h"

USING_3DF_NAMESPACE

class ConnectorFunc
{
public:
	Interface * GetInterface() { return m_pc3dfInterface; }

	Signal::Delivery & GetDelivery(int nViewId) {
		m_cDelivery.ViewId = nViewId;
		return m_cDelivery;
	}

	void SetSender(void (*func)(const wchar_t *)) {
		m_cDelivery.SetSender(func);
	}

private:
	Interface * m_pc3dfInterface = nullptr;
	Signal::Delivery m_cDelivery;
};

ConnectorFunc theConnectorFunc;

SendSignalFunc Connector::GetSender()
{
	return theConnectorFunc.GetInterface()->GetSendSignalFunc();
}

Signal::Delivery & Connector::GetInstance(int nViewId)
{
	return theConnectorFunc.GetDelivery(nViewId);
}

void Connector::SetSender(void (*func)(const wchar_t *))
{
	return theConnectorFunc.SetSender(func);
}