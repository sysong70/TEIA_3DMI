#include "StdAfx.h"

#include "KeyPath.h"

#include "Segment.h"

#include "Line.h"

#include <HTools.h>

USING_3DF_NAMESPACE

//== KeyPath Class =================================================================================

class KeyPathPrivate : public H3DF::PrivateImpl
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

H3DF::KeyPath::KeyPath()
{
	m_pcImpl = new KeyPathPrivate();
}

H3DF::KeyPath::KeyPath(KeyArray const & cInPath)
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInPath);
}

H3DF::KeyPath::KeyPath(size_t nInPathCount, Key const pInPath[])
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInPathCount, pInPath);
}

H3DF::KeyPath::KeyPath(size_t nInPathCount, HC_KEY const pInPath[])
{
	m_pcImpl = new KeyPathPrivate();

	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInPathCount, pInPath);
}

H3DF::KeyPath::KeyPath(char chKeyPath[])
{
	m_pcImpl = new KeyPathPrivate();
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	strcpy(pcImpl->m_chKeyPath, chKeyPath);
}

void H3DF::KeyPath::Set(KeyPath const & cInThat)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyPathPrivate * pcInThatImpl = (KeyPathPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

KeyPath & H3DF::KeyPath::operator = (KeyPath const & cInThat)
{
	Set(cInThat);
	return *this;
}

KeyPath & H3DF::KeyPath::operator = (KeyArray const & cInPath)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInPath);
	return *this;
}

KeyPath & H3DF::KeyPath::SetKeys(KeyArray const & cInKeys)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(cInKeys);
	return *this;
}

KeyPath & H3DF::KeyPath::SetKeys(size_t nInKeyCount, HC_KEY const pInKeys[])
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->Set(nInKeyCount, pInKeys);
	return *this;
}

KeyPath & H3DF::KeyPath::UnsetKeys()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.clear();
	pcImpl->m_vKeys.clear();
	return *this;
}

bool H3DF::KeyPath::ShowKeys(KeyArray & cOutKeys) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	if (true == pcImpl->m_aPaths.empty()) {
		return false;
	}

	cOutKeys = pcImpl->m_aPaths;

	return true;
}

bool H3DF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const
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

bool H3DF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, PointArray const & aInPoints, Coordinate::Space eInOutputSpace, PointArray & aOutPoints) const
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

bool H3DF::KeyPath::ShowNetModellingMatrix(MatrixKit & cOutKit) const
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

size_t H3DF::KeyPath::Size() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.size();
}

bool H3DF::KeyPath::Empty() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.empty();
}

Key & H3DF::KeyPath::At(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

Key const & H3DF::KeyPath::At(size_t nInIndex) const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

void H3DF::KeyPath::Insert(size_t nInIndex, Key const & cInItem)
{
	Insert(nInIndex, cInItem.KeyValue());
}

void H3DF::KeyPath::Insert(size_t nInIndex, HC_KEY nInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.insert(cIterator + nInIndex, nInKey);
}

void H3DF::KeyPath::Remove(Key const & cInItem)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;

	// 검색
	auto cIterator = std::find(pcImpl->m_aPaths.begin(), pcImpl->m_aPaths.end(), cInItem);

	// 검색에 성공하면 삭제
	if (cIterator != pcImpl->m_aPaths.end()) {
		pcImpl->m_aPaths.erase(cIterator);
	}
}

void H3DF::KeyPath::Remove(size_t nInIndex)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.erase(cIterator + nInIndex);
}

KeyPath H3DF::KeyPath::Reverse() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	KeyArray cReverseKeyArray = pcImpl->m_aPaths;

	std::reverse(cReverseKeyArray.begin(), cReverseKeyArray.end());

	return KeyPath(cReverseKeyArray);
}

Key & H3DF::KeyPath::Front()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key const & H3DF::KeyPath::Front() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key & H3DF::KeyPath::Back()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key const & H3DF::KeyPath::Back() const
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key H3DF::KeyPath::PopFront()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.front();
	pcImpl->m_aPaths.erase(pcImpl->m_aPaths.begin());
	return cKey;
}

Key H3DF::KeyPath::PopBack()
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.back();
	pcImpl->m_aPaths.pop_back();

	return cKey;
}

KeyPath & H3DF::KeyPath::PushFront(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.insert(pcImpl->m_aPaths.begin(), cInKey);
	return *this;
}

KeyPath & H3DF::KeyPath::PushBack(Key const & cInKey)
{
	KeyPathPrivate * pcImpl = (KeyPathPrivate *)m_pcImpl;
	pcImpl->m_aPaths.push_back(cInKey);
	return *this;
}