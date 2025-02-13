#include "StdAfx.h"

#include "KeyPath.h"

#include "Segment.h"

#include "Line.h"

#include "3DF.Utility.h"

#include <HTools.h>

using namespace H3DF;

//== KeyPath Class =================================================================================

namespace H3DF
{
	class KeyPathImpl : public H3DF::Impl
	{
	public:
		KeyPathImpl() { m_eType = H3DF::Type::KeyPath; }

		void Copy(KeyPathImpl * that)
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
				m_aPaths[nIndex].SetKeyValue(pInKeys[nIndex]);
				m_vKeys[nIndex] = pInKeys[nIndex];
			}
		}

		static bool GetCoordinateSpaceName(Coordinate::Space eInSpace, char chType[]);

		KeyArray m_aPaths;
		std::vector<HC_KEY> m_vKeys;
		char m_chKeyPath[MVO_BUFFER_SIZE] = "";
	};
}

bool H3DF::KeyPathImpl::GetCoordinateSpaceName(Coordinate::Space eInSpace, char chType[])
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
	m_pcImpl = new KeyPathImpl();
}

H3DF::KeyPath::KeyPath(KeyArray const & cInPath)
{
	m_pcImpl = new KeyPathImpl();

	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->Set(cInPath);
}

H3DF::KeyPath::KeyPath(size_t nInPathCount, Key const pInPath[])
{
	m_pcImpl = new KeyPathImpl();

	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->Set(nInPathCount, pInPath);
}

H3DF::KeyPath::KeyPath(KeyPath const & cInThat)
{
	KeyPathImpl * pcImpl = new KeyPathImpl();
	DEBUG_VALID(pcImpl);
	m_pcImpl = pcImpl;

	KeyPathImpl * pcInThatImpl = (KeyPathImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

H3DF::KeyPath::KeyPath(char chKeyPath[])
{
	m_pcImpl = new KeyPathImpl();
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;

	strcpy(pcImpl->m_chKeyPath, chKeyPath);
}

void H3DF::KeyPath::Set(KeyPath const & cInThat)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	KeyPathImpl * pcInThatImpl = (KeyPathImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

KeyPath & H3DF::KeyPath::operator = (KeyPath const & cInThat)
{
	Set(cInThat);
	return *this;
}

KeyPath & H3DF::KeyPath::operator = (KeyArray const & cInPath)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->Set(cInPath);
	return *this;
}

KeyPath & H3DF::KeyPath::SetKeys(KeyArray const & cInKeys)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->Set(cInKeys);
	return *this;
}

KeyPath & H3DF::KeyPath::UnsetKeys()
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->m_aPaths.clear();
	pcImpl->m_vKeys.clear();
	return *this;
}

bool H3DF::KeyPath::ShowKeys(KeyArray & cOutKeys) const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	if (true == pcImpl->m_aPaths.empty()) {
		return false;
	}

	cOutKeys = pcImpl->m_aPaths;

	return true;
}

bool H3DF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;

	char chInSpaceName[64];
	char chInOutputSpaceName[64];

	H3DF::KeyPathImpl::GetCoordinateSpaceName(eInSpace, chInSpaceName);
	H3DF::KeyPathImpl::GetCoordinateSpaceName(eInOutputSpace, chInOutputSpaceName);

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
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;

	aOutPoints.resize(aInPoints.size());

	char chInSpaceName[64];
	char chInOutputSpaceName[64];

	H3DF::KeyPathImpl::GetCoordinateSpaceName(eInSpace, chInSpaceName);
	H3DF::KeyPathImpl::GetCoordinateSpaceName(eInOutputSpace, chInOutputSpaceName);

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
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	if (true == pcImpl->m_vKeys.empty()) {
		return false;
	}

	float pfMatrix[16];
 	if (0 == HC_PShow_Net_Modelling_Matrix((int) pcImpl->m_vKeys.size(), pcImpl->m_vKeys.data(), pfMatrix)) {
 		return false;
 	}

	cOutKit = MatrixKit(pfMatrix);

	return true;
}

size_t H3DF::KeyPath::Size() const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.size();
}

bool H3DF::KeyPath::Empty() const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.empty();
}

Key & H3DF::KeyPath::At(size_t nInIndex)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

Key const & H3DF::KeyPath::At(size_t nInIndex) const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.at(nInIndex);
}

void H3DF::KeyPath::Insert(size_t nInIndex, Key const & cInItem)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *) m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.insert(cIterator + nInIndex, cInItem);
}

void H3DF::KeyPath::Remove(Key const & cInItem)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;

	// 검색
	auto cIterator = std::find(pcImpl->m_aPaths.begin(), pcImpl->m_aPaths.end(), cInItem);

	// 검색에 성공하면 삭제
	if (cIterator != pcImpl->m_aPaths.end()) {
		pcImpl->m_aPaths.erase(cIterator);
	}
}

void H3DF::KeyPath::Remove(size_t nInIndex)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.erase(cIterator + nInIndex);
}

KeyPath H3DF::KeyPath::Reverse() const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	KeyArray cReverseKeyArray = pcImpl->m_aPaths;

	std::reverse(cReverseKeyArray.begin(), cReverseKeyArray.end());

	return KeyPath(cReverseKeyArray);
}

Key & H3DF::KeyPath::Front()
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key const & H3DF::KeyPath::Front() const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.front();
}

Key & H3DF::KeyPath::Back()
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key const & H3DF::KeyPath::Back() const
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	return pcImpl->m_aPaths.back();
}

Key H3DF::KeyPath::PopFront()
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.front();
	pcImpl->m_aPaths.erase(pcImpl->m_aPaths.begin());
	return cKey;
}

Key H3DF::KeyPath::PopBack()
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	Key cKey = pcImpl->m_aPaths.back();
	pcImpl->m_aPaths.pop_back();

	return cKey;
}

KeyPath & H3DF::KeyPath::PushFront(Key const & cInKey)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->m_aPaths.insert(pcImpl->m_aPaths.begin(), cInKey);
	return *this;
}

KeyPath & H3DF::KeyPath::PushBack(Key const & cInKey)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *)m_pcImpl;
	pcImpl->m_aPaths.push_back(cInKey);
	return *this;
}

void H3DF::KeyPath::ShowString(CString & strOutPath)
{
	KeyPathImpl * pcImpl = (KeyPathImpl *) m_pcImpl;

	CString strText;

	size_t nIndex = 0;

	for (auto & cKey : pcImpl->m_aPaths) {
		H3DF::Type eType = Utility::GetType(cKey);
		HC_KEY nKey = cKey.KeyValue();

		if (H3DF::Type::SegmentKey == eType) {
			SegmentKey cSegment(nKey);

			CString strUserName = L"_None_";
			UserData::ShowSegmentName(cSegment, strUserName);

			strText.Format(L"\n%d. Segment: %d [%s, %s]", nIndex, nKey, strUserName, CString(cSegment.Name(false)));
		}
		else {

			strText.Format(L"\n%d. %s: %d", nIndex, H3DF::Utility::GetTypeString(eType), nKey);
		}

		strOutPath += strText;
		nIndex++;
	}
}