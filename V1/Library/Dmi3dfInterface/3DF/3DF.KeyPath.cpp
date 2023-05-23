#include "StdAfx.h"

#include "3DF.KeyPath.h"

#include "3DF.Segment.h"
#include "3DF.Selectability.h"

#include "3DF.Line.h"

#include <HTools.h>

USING_3DF_NAMESPACE

//== KeyPath Class =================================================================================

class KeyPathPrivate : public TDF::PrivateImpl
{
public:
	void Copy(KeyPathPrivate * that)
	{
		m_aPaths = that->m_aPaths;
		m_vKeys = that->m_vKeys;
		strcpy(that->m_chKeyPath, m_chKeyPath);
	}

	void Set(KeyArray const & cInKeys)
	{
		m_aPaths = cInKeys;
		m_vKeys.resize(m_aPaths.size());

		for (size_t nIndex = 0; nIndex < m_aPaths.size(); nIndex++) {
			m_vKeys[nIndex] = m_aPaths[nIndex].KeyValue();
		}
	}

	void Set(std::vector<HC_KEY> vInKeys)
	{
		m_vKeys = vInKeys;
		m_aPaths.resize(m_vKeys.size());

		for (size_t nIndex = 0; nIndex < m_vKeys.size(); nIndex++) {
			m_aPaths[nIndex].SetKeyValue(m_vKeys[nIndex]);
		}
	}

	void Set(size_t nInPathCount, Key const pInPath[])
	{
		m_aPaths.resize(nInPathCount);
		m_vKeys.resize(nInPathCount);
		for (size_t nIndex = 0; nIndex < m_vKeys.size(); nIndex++) {
			m_aPaths[nIndex] = pInPath[nIndex];
			m_vKeys[nIndex] = pInPath[nIndex].KeyValue();
		}
	}

	void Set(size_t nInKeyCount, HC_KEY const pInKeys[])
	{
		m_aPaths.resize(nInKeyCount);
		m_vKeys.resize(nInKeyCount);

		for (size_t nIndex = 0; nIndex < m_vKeys.size(); nIndex++) {
			m_aPaths[nIndex].SetKeyValue(m_vKeys[nIndex]);
			m_vKeys[nIndex] = pInKeys[nIndex];
		}
	}

	static bool GetCoordinateSpaceName(Coordinate::Space eInSpace, char chType[]);

	KeyArray m_aPaths;
	std::vector<HC_KEY> m_vKeys;
	char m_chKeyPath[MVO_BUFFER_SIZE] = "";
};

bool KeyPathPrivate::GetCoordinateSpaceName(Coordinate::Space eInSpace, char chType[])
{
	if (nullptr == chType) {
		return false;
	}

	switch (eInSpace) {
		case Coordinate::Space::Object:
			sprintf(chType, "object");
		break;

		case Coordinate::Space::World:
			sprintf(chType, "world");
			break;

		case Coordinate::Space::Camera:
			sprintf(chType, "camera");
		break;

		case Coordinate::Space::Window:
			sprintf(chType, "outer window");
		break;

		case Coordinate::Space::Pixel:
			sprintf(chType, "outer pixel");
		break;

		case Coordinate::Space::InnerWindow:
			sprintf(chType, "local window");
		break;

		case Coordinate::Space::InnerPixel:
			sprintf(chType, "local pixel");
		break;

		case Coordinate::Space::ScreenRange:
			sprintf(chType, "screen range");
		break;
	}

	return true;
}

TDF::KeyPath::KeyPath()
{
	m_pcImpl = new KeyPathPrivate();
}

TDF::KeyPath::KeyPath(KeyArray const & cInPath)
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInPath);
}

TDF::KeyPath::KeyPath(size_t nInPathCount, Key const pInPath[])
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInPathCount, pInPath);
}

TDF::KeyPath::KeyPath(size_t nInPathCount, HC_KEY const pInPath[])
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInPathCount, pInPath);
}

TDF::KeyPath::KeyPath(char chKeyPath[])
{
	m_pcImpl = new KeyPathPrivate();
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	strcpy(pcImpl->m_chKeyPath, chKeyPath);
}

void TDF::KeyPath::Set(KeyPath const & cInThat)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyPathPrivate * pcInThatImpl = (KeyPathPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

KeyPath & TDF::KeyPath::operator = (KeyPath const & cInThat)
{
	Set(cInThat);
	return *this;
}

KeyPath & TDF::KeyPath::operator = (KeyArray const & cInPath)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInPath);
	return *this;
}

KeyPath & TDF::KeyPath::SetKeys(KeyArray const & cInKeys)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInKeys);
	return *this;
}

KeyPath & TDF::KeyPath::SetKeys(size_t nInKeyCount, HC_KEY const pInKeys[])
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInKeyCount, pInKeys);
	return *this;
}

KeyPath & TDF::KeyPath::UnsetKeys()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.clear();
	pcImpl->m_vKeys.clear();
	return *this;
}

bool TDF::KeyPath::ShowKeys(KeyArray & cOutKeys) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	if (true == pcImpl->m_aPaths.empty()) {
		return false;
	}

	cOutKeys = pcImpl->m_aPaths;

	return true;
}

bool TDF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	char chInSpaceName[64];
	char chInOutputSpaceName[64];

	KeyPathPrivate::GetCoordinateSpaceName(eInSpace, chInSpaceName);
	KeyPathPrivate::GetCoordinateSpaceName(eInOutputSpace, chInOutputSpaceName);

	if (0 < strlen(pcImpl->m_chKeyPath)) {
		if (TRUE == HC_Compute_Coordinates(pcImpl->m_chKeyPath, chInSpaceName, &cInpoint, chInOutputSpaceName, &cOutPoint)) {
			return true;
		}
	}
	else {
		if (true == pcImpl->m_vKeys.empty()) {
			return false;
		}

		if (TRUE == HC_Compute_Coordinates_By_Path((int)pcImpl->m_vKeys.size(), pcImpl->m_vKeys.data(), chInSpaceName, &cInpoint, chInOutputSpaceName, &cOutPoint)) {
			return true;
		}
	}

	return false;
}

bool TDF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, PointArray const & aInPoints, Coordinate::Space eInOutputSpace, PointArray & aOutPoints) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	aOutPoints.resize(aInPoints.size());

	char chInSpaceName[64];
	char chInOutputSpaceName[64];

	KeyPathPrivate::GetCoordinateSpaceName(eInSpace, chInSpaceName);
	KeyPathPrivate::GetCoordinateSpaceName(eInOutputSpace, chInOutputSpaceName);

	if (0 < strlen(pcImpl->m_chKeyPath)) {
		for (size_t nIndex = 0; nIndex < aInPoints.size(); nIndex++) {
			if (FALSE == HC_Compute_Coordinates(pcImpl->m_chKeyPath, chInSpaceName, &aInPoints[nIndex], chInOutputSpaceName, &aOutPoints[nIndex])) {
				return false;
			}
		}
	}
	else {
		if (true == pcImpl->m_vKeys.empty()) {
			return false;
		}

		for (size_t nIndex = 0; nIndex < aInPoints.size(); nIndex++) {
			if (FALSE == HC_Compute_Coordinates_By_Path((int)pcImpl->m_vKeys.size(), pcImpl->m_vKeys.data(), chInSpaceName, &aInPoints[nIndex], chInOutputSpaceName, &aOutPoints[nIndex])) {
				return false;
			}
		}
	}

	return true;
}

bool TDF::KeyPath::ShowNetSelectability(SelectabilityKit & cOutKit) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	if (true == pcImpl->m_vKeys.empty()) {
		return false;
	}

	char pchSelectability[MVO_BUFFER_SIZE];
	if (0 == HC_PShow_Net_Selectability((int)pcImpl->m_aPaths.size(), pcImpl->m_vKeys.data(), pchSelectability)) {
		return false;
	}

	cOutKit.Set(pchSelectability);

	return true;
}

bool TDF::KeyPath::ShowNetModellingMatrix(MatrixKit & cOutKit) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	if (true == pcImpl->m_vKeys.empty()) {
		return false;
	}

	float pfMatrix[16];
	if (0 == HC_PShow_Net_Modelling_Matrix((int)pcImpl->m_aPaths.size(), pcImpl->m_vKeys.data(), pfMatrix)) {
		return false;
	}

	cOutKit = MatrixKit(pfMatrix);

	return true;
}

size_t TDF::KeyPath::Size() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.size();
}

bool TDF::KeyPath::Empty() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.empty();
}

Key & TDF::KeyPath::At(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

Key const & TDF::KeyPath::At(size_t nInIndex) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

void TDF::KeyPath::Insert(size_t nInIndex, Key const & cInItem)
{
	Insert(nInIndex, cInItem.KeyValue());
}

void TDF::KeyPath::Insert(size_t nInIndex, HC_KEY nInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.insert(cIterator + nInIndex, nInKey);
}

void TDF::KeyPath::Remove(Key const & cInItem)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	// 검색
	auto cIterator = std::find(pcImpl->m_aPaths.begin(), pcImpl->m_aPaths.end(), cInItem);

	// 검색에 성공하면 삭제
	if (cIterator != pcImpl->m_aPaths.end()) {
		pcImpl->m_aPaths.erase(cIterator);
	}
}

void TDF::KeyPath::Remove(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.erase(cIterator + nInIndex);
}

KeyPath TDF::KeyPath::Reverse() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyArray cReverseKeyArray = pcImpl->m_aPaths;

	std::reverse(cReverseKeyArray.begin(), cReverseKeyArray.end());

	return KeyPath(cReverseKeyArray);
}

Key & TDF::KeyPath::Front()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key const & TDF::KeyPath::Front() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key & TDF::KeyPath::Back()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key const & TDF::KeyPath::Back() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key TDF::KeyPath::PopFront()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.front();
	pcImpl->m_aPaths.erase(pcImpl->m_aPaths.begin());
	return cKey;
}

Key TDF::KeyPath::PopBack()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.back();
	pcImpl->m_aPaths.pop_back();

	return cKey;
}

KeyPath & TDF::KeyPath::PushFront(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.insert(pcImpl->m_aPaths.begin(), cInKey);
	return *this;
}

KeyPath & TDF::KeyPath::PushBack(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.push_back(cInKey);
	return *this;
}