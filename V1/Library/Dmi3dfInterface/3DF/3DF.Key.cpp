#include "StdAfx.h"

#include "3DF.Key.h"
#include "Private/3DF.KeyPrivate.h"

#include "3DF.Segment.h"
#include "3DF.Selectability.h"

#include "3DF.Line.h"

#include <HTools.h>

USING_3DF_NAMESPACE

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
}

void Key::Set(Key const & cInThat)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Key const & Key::operator = (Key const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool Key::operator == (Key const & cInThat) const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	return (pcImpl->m_nKey == pcInThatImpl->m_nKey);
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

void Key::Delete()
{
	assert(false);
}

//== KeyPath Class =================================================================================

class KeyPathPrivate : public TDF::PrivateImpl
{
public:
	void Copy(KeyPathPrivate * that)
	{
		m_aPaths = that->m_aPaths;
	}

	HC_KEY * PathKeys() const
	{
		if (true == m_aPaths.empty()) {
			return nullptr;
		}

		HC_KEY * pbPathKeys = new HC_KEY[m_aPaths.size()];
		if (nullptr == pbPathKeys) {
			return nullptr;
		}

		for (int i = 0; i < m_aPaths.size(); i++) {
			pbPathKeys[i] = m_aPaths[i].KeyValue();
		}

		return pbPathKeys;
	}

	KeyArray m_aPaths;
};

KeyPath::KeyPath()
{
	m_pcImpl = new KeyPathPrivate();
}

KeyPath::KeyPath(KeyArray const & cInPath)
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths = cInPath;
}

KeyPath::KeyPath(size_t nInPathCount, HC_KEY const pInPath[])
{
	m_pcImpl = new KeyPathPrivate();
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	pcImpl->m_aPaths.resize(nInPathCount);

	for (size_t nIndex = 0; nIndex < nInPathCount; nIndex++) {
		pcImpl->m_aPaths[nIndex] = pInPath[nIndex];
	}
}

void KeyPath::Set(KeyPath const & cInThat)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyPathPrivate * pcInThatImpl = (KeyPathPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

KeyPath & KeyPath::operator = (KeyPath const & cInThat)
{
	Set(cInThat);
	return *this;
}

KeyPath & KeyPath::operator = (KeyArray const & cInPath)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths = cInPath;
	return *this;
}

KeyPath & KeyPath::SetKeys(KeyArray const & cInKeys)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths = cInKeys;
	return *this;
}

KeyPath & KeyPath::SetKeys(size_t nInKeyCount, HC_KEY const pInKeys[])
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.resize(nInKeyCount);

	for (size_t nIndex = 0 ; nIndex < nInKeyCount; nIndex++) {
		pcImpl->m_aPaths[nIndex] = pInKeys[nIndex];
	}

	return *this;
}

KeyPath & KeyPath::UnsetKeys()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.clear();

	return *this;
}

bool KeyPath::ShowKeys(KeyArray & cOutKeys) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	if (true == pcImpl->m_aPaths.empty()) {
		return false;
	}

	cOutKeys = pcImpl->m_aPaths;

	return true;
}

bool KeyPath::ShowNetSelectability(SelectabilityKit & cOutKit) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	HC_KEY * pbPathKeys = pcImpl->PathKeys();
	if (nullptr == pbPathKeys) {
		return false;
	}

	char pchSelectability[MVO_BUFFER_SIZE];
	if (0 == HC_PShow_Net_Selectability((int)pcImpl->m_aPaths.size(), pbPathKeys, pchSelectability)) {
		return false;
	}

	cOutKit.Set(pchSelectability);

	return true;
}

bool KeyPath::ShowNetModellingMatrix(MatrixKit & cOutKit) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	HC_KEY * pbPathKeys = pcImpl->PathKeys();
	if (nullptr == pbPathKeys) {
		return false;
	}

	float pfMatrix[16];
	if(0 == HC_PShow_Net_Modelling_Matrix((int)pcImpl->m_aPaths.size(), pbPathKeys, pfMatrix)) {
		return false;
	}

	cOutKit = MatrixKit(pfMatrix);

	return true;
}

size_t KeyPath::Size() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.size();
}

bool KeyPath::Empty() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.empty();
}

Key & KeyPath::At(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

Key const & KeyPath::At(size_t nInIndex) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

void KeyPath::Insert(size_t nInIndex, Key const & cInItem)
{
	Insert(nInIndex, cInItem.KeyValue());
}

void KeyPath::Insert(size_t nInIndex, HC_KEY nInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.insert(cIterator + nInIndex, nInKey);
}

void KeyPath::Remove(Key const & cInItem)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	// 검색
	auto cIterator = std::find(pcImpl->m_aPaths.begin(), pcImpl->m_aPaths.end(), cInItem);

	// 검색에 성공하면 삭제
	if (cIterator != pcImpl->m_aPaths.end()) {
		pcImpl->m_aPaths.erase(cIterator);
	}
}

void KeyPath::Remove(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.erase(cIterator + nInIndex);
}

KeyPath KeyPath::Reverse() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyArray cReverseKeyArray = pcImpl->m_aPaths;

	std::reverse(cReverseKeyArray.begin(), cReverseKeyArray.end());

	return KeyPath(cReverseKeyArray);
}

Key & KeyPath::Front()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key const & KeyPath::Front() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key & KeyPath::Back()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key const & KeyPath::Back() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key KeyPath::PopFront()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.front();
	pcImpl->m_aPaths.erase(pcImpl->m_aPaths.begin());
	return cKey;
}

Key KeyPath::PopBack()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.back();
	pcImpl->m_aPaths.pop_back();

	return cKey;
}

KeyPath & KeyPath::PushFront(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.insert(pcImpl->m_aPaths.begin(), cInKey);
	return *this;
}

KeyPath & KeyPath::PushBack(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.push_back(cInKey);
	return *this;
}