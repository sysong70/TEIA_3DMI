#include "StdAfx.h"

#include "3DF.Key.h"
#include "3DF.Segment.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class KeyPrivate : public _3DF::PrivateImpl
{
public:
	KeyPrivate() {}
	~KeyPrivate() {}

	HC_KEY m_nKey = INVALID_KEY;
	HC_KEY m_nOwnerKey = INVALID_KEY;

	bool m_bOpen = false;
	bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.

	void Copy(KeyPrivate * pcInThat)
	{
		m_nKey = pcInThat->m_nKey;
		m_nOwnerKey = pcInThat->m_nOwnerKey;

		m_bOpen = pcInThat->m_bOpen;
		m_bForcedOpen = pcInThat->m_bForcedOpen;
	}
};

Key::Key(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = new KeyPrivate();
	pcImpl->m_nKey = nInKey;

	m_pcImpl = pcImpl;
}

Key::Key(Key const & cInThat)
{
	m_pcImpl = new KeyPrivate();
	Set(cInThat);
}

Key::~Key()
{
/*
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if(true == pcImpl->m_bForcedOpen) {
		return;
	}

	if(true == pcImpl->m_bOpen) {
		HC_Close_Segment();
	}*/
}

void Key::Set(Key const & cInThat)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Key const & Key::operator=(Key const & cInThat)
{
	Set(cInThat);
	return *this;
}

void Key::Open()
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if(true == pcImpl->m_bForcedOpen) {
		return;
	}

	if(true == pcImpl->m_bOpen) {
		return;
	}

	pcImpl->m_bOpen = true;
	assert(INVALID_KEY != pcImpl->m_nKey);
	HC_Open_Segment_By_Key(pcImpl->m_nKey);
}

void Key::Open() const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if (true == pcImpl->m_bForcedOpen) {
		return;
	}

	if (true == pcImpl->m_bOpen) {
		return;
	}

	pcImpl->m_bOpen = true;
	assert(INVALID_KEY != pcImpl->m_nKey);
	HC_Open_Segment_By_Key(pcImpl->m_nKey);
}

void Key::Close()
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if(true == pcImpl->m_bForcedOpen) {
		return;
	}

	if(false == pcImpl->m_bOpen) {
		return;
	}

	pcImpl->m_bOpen = false;
	assert(INVALID_KEY != pcImpl->m_nKey);
	HC_Close_Segment();
}

void Key::Close() const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if (true == pcImpl->m_bForcedOpen) {
		return;
	}

	if (false == pcImpl->m_bOpen) {
		return;
	}

	pcImpl->m_bOpen = false;
	assert(INVALID_KEY != pcImpl->m_nKey);
	HC_Close_Segment();
}

void Key::ForcedOpen()
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	pcImpl->m_bForcedOpen = true;

	if(true == pcImpl->m_bOpen) {
		return;
	}
	
	pcImpl->m_bOpen = true;
	HC_Open_Segment_By_Key(pcImpl->m_nKey);
}

void Key::ForcedClose()
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if(false == pcImpl->m_bOpen) {
		return;
	}

	pcImpl->m_bForcedOpen = false;
	pcImpl->m_bOpen = false;
	HC_Close_Segment();
}

HC_KEY Key::KeyValue() const 
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return pcImpl->m_nKey;
}

void Key::SetKeyValue(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}

void Key::SetKeyValue(HC_KEY nInKey) const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}


bool Key::IsOpen() const 
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return pcImpl->m_bOpen;
}

bool Key::IsForcedOpen() const 
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return pcImpl->m_bForcedOpen;
}

bool Key::HasOwner() const
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return (INVALID_KEY != pcImpl->m_nOwnerKey) ? true : false;
}

void Key::SetOwerKey(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nOwnerKey = nInKey;
}

SegmentKey Key::Owner()
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;

	if(INVALID_KEY == pcImpl->m_nKey && INVALID_KEY == pcImpl->m_nOwnerKey) {
		assert(false);
	}

	if(INVALID_KEY == pcImpl->m_nOwnerKey) {
		Open();
		pcImpl->m_nOwnerKey = HC_Show_Owner_Original_Key(pcImpl->m_nKey);
		Close();
	}

	SegmentKey cOwner(pcImpl->m_nOwnerKey);
	return cOwner;
}

void Key::Delete()
{
	assert(false);
}