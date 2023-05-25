#include "StdAfx.h"

#include "3DF.Key.h"
#include "Private/3DF.KeyPrivate.h"

#include "3DF.Segment.h"
#include "3DF.Selectability.h"

#include "3DF.Line.h"

#include <HTools.h>

USING_3DF_NAMESPACE

TDF::Key::Key(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = new KeyPrivate();
	pcImpl->m_nKey = nInKey;

	m_pcImpl = pcImpl;
}

TDF::Key::Key(Key const & cInThat)
{
	m_pcImpl = new KeyPrivate();
	Set(cInThat);
}

TDF::Key::~Key()
{
}

void TDF::Key::Set(Key const & cInThat)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Key const & TDF::Key::operator = (Key const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool TDF::Key::operator == (Key const & cInThat) const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	return (pcImpl->m_nKey == pcInThatImpl->m_nKey);
}

HC_KEY TDF::Key::KeyValue() const 
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return pcImpl->m_nKey;
}

void TDF::Key::SetKeyValue(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}

void TDF::Key::SetKeyValue(HC_KEY nInKey) const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}

void TDF::Key::Delete()
{
	assert(false);
}

bool TDF::Key::HasOwner() const
{
	HC_KEY nOwnerKey  = HC_Show_Owner_Original_Key(KeyValue());
	if (INVALID_KEY == nOwnerKey) {
		return false;
	}

	return true;
}

// return: The segment containing this key.
SegmentKey TDF::Key::Up() const
{
	return Owner();
}

// return: The segment containing this key.
SegmentKey TDF::Key::Owner() const
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