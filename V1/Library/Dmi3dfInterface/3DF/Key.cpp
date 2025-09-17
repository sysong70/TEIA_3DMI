#include "StdAfx.h"

#include "Key.h"
#include "Impl/KeyImpl.h"

#include "Segment.h"

#include "Line.h"

#include <HTools.h>

#include <stdexcept>

using namespace H3DF;

H3DF::Key::Key() 
{
}

H3DF::Key::Key(HC_KEY nInKey)
{
	if (INVALID_KEY == nInKey) {
		DEBUG_RETURN;
	}

	m_pcImpl = (INVALID_KEY == nInKey) ? nullptr : std::make_unique<KeyImpl>();
	DEBUG_VALID(m_pcImpl);

	static_cast<KeyImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);
}

H3DF::Key::Key(Key const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

Key const & H3DF::Key::operator = (Key const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::Key::operator == (Key const & cInThat) const
{
	auto pcImpl = dynamic_cast<KeyImpl *>(m_pcImpl.get());
	auto pcInThatImpl = dynamic_cast<KeyImpl *>(cInThat.m_pcImpl.get());

	if (nullptr == pcImpl || nullptr == pcInThatImpl) {
		DEBUG_STOP;
		return false;
	}

	return pcImpl->KeyValue() == pcInThatImpl->KeyValue();
}

bool H3DF::Key::IsValidate() const
{
	auto pcImpl = dynamic_cast<KeyImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	return (INVALID_KEY != pcImpl->KeyValue());
}

HC_KEY H3DF::Key::KeyValue() const 
{ 
	auto pcImpl = dynamic_cast<KeyImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return INVALID_KEY;
	}

	return pcImpl->KeyValue();
}

void H3DF::Key::SetKeyValue(HC_KEY nInKey)
{
	auto pcImpl = static_cast<KeyImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return;
	}

	pcImpl->SetKeyValue(nInKey);
}

void H3DF::Key::SetKeyValue(HC_KEY nInKey) const
{
	auto pcImpl = dynamic_cast<KeyImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return;
	}

	pcImpl->SetKeyValue(nInKey);
}

void H3DF::Key::Delete()
{
	HC_KEY nKey = KeyValue();
	if (INVALID_KEY == nKey) {
		DEBUG_STOP;
		return;
	}

	HC_Delete_By_Key(nKey);

	SetKeyValue(INVALID_KEY);
}

void H3DF::Key::MoveTo(SegmentKey const & cInNewOwner)
{
	HC_KEY nKey = KeyValue();
	if (INVALID_KEY == nKey) {
		DEBUG_STOP;
		return;
	}

	HC_KEY nNewOnwerKey = cInNewOwner.KeyValue();
	if (INVALID_KEY == nNewOnwerKey) {
		DEBUG_STOP;
		return;
	}

	HC_Move_Key_By_Key(nKey, nNewOnwerKey);
}

bool H3DF::Key::HasOwner() const
{
	HC_KEY nOwnerKey  = HC_Show_Owner_Original_Key(KeyValue());
	if (INVALID_KEY == nOwnerKey) {
		return false;
	}

	return true;
}

// return: The segment containing this key.
SegmentKey H3DF::Key::Up() const
{
	return Owner();
}

// return: The segment containing this key.
SegmentKey H3DF::Key::Owner() const
{
	HC_KEY nOwnerKey = HC_Show_Owner_Original_Key(KeyValue());
	if (INVALID_KEY == nOwnerKey) {
		return {};
	}

	// Onwer Key가 Segmnet인지 여부 확인.
	char chType[MVO_BUFFER_SIZE]{};
	HC_Show_Key_Type(nOwnerKey, chType);
	if (0 != strcmp(chType, "segment")) {
		return {};
	}

	SegmentKey cOwner(nOwnerKey);
	return cOwner;
}