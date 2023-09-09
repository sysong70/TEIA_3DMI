#include "StdAfx.h"

#include "Signal.Connector.h"

using namespace KERNEL;

class ConnectorFunc
{
public:
	Signal::Delivery & GetDelivery(int nViewId) {
		m_cDelivery.ViewId = nViewId;
		return m_cDelivery;
	}

	void SetSender(void (*func)(const wchar_t *)) {
		m_cDelivery.SetSender(func);
	}

private:
	Signal::Delivery m_cDelivery;
};

ConnectorFunc theConnectorFunc;

Signal::Delivery & Connector::GetInstance(int nViewId)
{
	return theConnectorFunc.GetDelivery(nViewId);
}

void Connector::SetSender(void (*func)(const wchar_t *))
{
	return theConnectorFunc.SetSender(func);
}