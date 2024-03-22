#include "StdAfx.h"

#include "Key.h"
#include "Impl/KeyImpl.h"

#include "Segment.h"

#include "Line.h"

#include <HTools.h>

using namespace H3DF;

H3DF::Key::Key() 
{
	m_pcImpl = new KeyImpl();
}

H3DF::Key::Key(HC_KEY nInKey)
{
	if (INVALID_KEY == nInKey) {
		return;
	}

	KeyImpl * pcImpl = new KeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::Key::Key(Key const & cInThat)
{
	if (INVALID_KEY == cInThat.KeyValue()) {
		return;
	}

	m_pcImpl = new KeyImpl();
	Set(cInThat);
}

void H3DF::Key::Set(Key const & cInThat)
{
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	KeyImpl * pcInThatImpl = (KeyImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Key const & H3DF::Key::operator = (Key const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::Key::operator == (Key const & cInThat) const
{
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	KeyImpl * pcInThatImpl = (KeyImpl *)cInThat.m_pcImpl;
	return (pcImpl->KeyValue() == pcInThatImpl->KeyValue());
}

HC_KEY H3DF::Key::KeyValue() const 
{ 
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	return pcImpl->KeyValue();
}

void H3DF::Key::SetKeyValue(HC_KEY nInKey)
{
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	pcImpl->SetKeyValue(nInKey);
}

void H3DF::Key::SetKeyValue(HC_KEY nInKey) const
{
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	pcImpl->SetKeyValue(nInKey);
}

void H3DF::Key::Delete()
{
	KeyImpl * pcImpl = (KeyImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_Delete_By_Key(pcImpl->KeyValue());

	pcImpl->SetKeyValue(INVALID_KEY);
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
	SegmentKey cOwner;

	HC_KEY nOwnerKey = HC_Show_Owner_Original_Key(KeyValue());
	if (INVALID_KEY == nOwnerKey) {
		return cOwner;
	}

	// Onwer Key가 Segmnet인지 여부 확인.
	char chType[MVO_BUFFER_SIZE];
	HC_Show_Key_Type(nOwnerKey, chType);
	if (0 != strcmp(chType, "segment")) {
		return cOwner;
	}

	cOwner.SetKeyValue(nOwnerKey);
	return cOwner;
}