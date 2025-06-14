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
		std::unique_ptr<Impl> Clone() const
		{
			// 새 객체 생성
			auto pcClone = std::make_unique<KeyPathImpl>();

			pcClone->m_aPaths = m_aPaths;
			pcClone->m_vKeys = m_vKeys;

			strcpy_s(pcClone->m_chKeyPath, sizeof(pcClone->m_chKeyPath), m_chKeyPath);

			return pcClone;

		}

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
	m_pcImpl = std::make_unique<KeyPathImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::KeyPath::KeyPath(KeyArray const & cInPath)
{
	m_pcImpl = std::make_unique<KeyPathImpl>();
	static_cast<KeyPathImpl *>(m_pcImpl.get())->Set(cInPath);
}

H3DF::KeyPath::KeyPath(size_t nInPathCount, Key const pInPath[])
{
	m_pcImpl = std::make_unique<KeyPathImpl>();
	static_cast<KeyPathImpl *>(m_pcImpl.get())->Set(nInPathCount, pInPath);
}

H3DF::KeyPath::KeyPath(KeyPath const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::KeyPath::KeyPath(char chKeyPath[])
{
	m_pcImpl = std::make_unique<KeyPathImpl>();
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

	// 안전하게 복사
	strcpy_s(pcImpl->m_chKeyPath, sizeof(pcImpl->m_chKeyPath), chKeyPath);
}

KeyPath & H3DF::KeyPath::operator = (KeyPath const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

KeyPath & H3DF::KeyPath::operator = (KeyArray const & cInPath)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) {
		// 방어 코드: 만약 생성자에서 m_pcImpl 할당 안 된 경우
		m_pcImpl = std::make_unique<KeyPathImpl>();
		pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	}

	pcImpl->Set(cInPath);

	return *this;
}

KeyPath & H3DF::KeyPath::SetKeys(KeyArray const & cInKeys)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	pcImpl->Set(cInKeys);

	return *this;
}

KeyPath & H3DF::KeyPath::UnsetKeys()
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	pcImpl->m_aPaths.clear();
	pcImpl->m_vKeys.clear();
	return *this;
}

bool H3DF::KeyPath::ShowKeys(KeyArray & cOutKeys) const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	if (true == pcImpl->m_aPaths.empty()) {
		return false;
	}

	cOutKeys = pcImpl->m_aPaths;

	return true;
}

bool H3DF::KeyPath::ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

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
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

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
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

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
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.size();
}

bool H3DF::KeyPath::Empty() const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.empty();
}

Key & H3DF::KeyPath::At(size_t nInIndex)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.at(nInIndex);
}

Key const & H3DF::KeyPath::At(size_t nInIndex) const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.at(nInIndex);
}

void H3DF::KeyPath::Insert(size_t nInIndex, Key const & cInItem)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.insert(cIterator + nInIndex, cInItem);
}

void H3DF::KeyPath::Remove(Key const & cInItem)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

	// 검색
	auto cIterator = std::find(pcImpl->m_aPaths.begin(), pcImpl->m_aPaths.end(), cInItem);

	// 검색에 성공하면 삭제
	if (cIterator != pcImpl->m_aPaths.end()) {
		pcImpl->m_aPaths.erase(cIterator);
	}
}

void H3DF::KeyPath::Remove(size_t nInIndex)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	auto cIterator = pcImpl->m_aPaths.begin();
	pcImpl->m_aPaths.erase(cIterator + nInIndex);
}

KeyPath H3DF::KeyPath::Reverse() const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	KeyArray cReverseKeyArray = pcImpl->m_aPaths;

	std::reverse(cReverseKeyArray.begin(), cReverseKeyArray.end());

	return KeyPath(cReverseKeyArray);
}

Key & H3DF::KeyPath::Front()
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.front();
}

Key const & H3DF::KeyPath::Front() const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.front();
}

Key & H3DF::KeyPath::Back()
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.back();
}

Key const & H3DF::KeyPath::Back() const
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	return pcImpl->m_aPaths.back();
}

Key H3DF::KeyPath::PopFront()
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	Key cKey = pcImpl->m_aPaths.front();
	pcImpl->m_aPaths.erase(pcImpl->m_aPaths.begin());
	return cKey;
}

Key H3DF::KeyPath::PopBack()
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	Key cKey = pcImpl->m_aPaths.back();
	pcImpl->m_aPaths.pop_back();

	return cKey;
}

KeyPath & H3DF::KeyPath::PushFront(Key const & cInKey)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	pcImpl->m_aPaths.insert(pcImpl->m_aPaths.begin(), cInKey);
	return *this;
}

KeyPath & H3DF::KeyPath::PushBack(Key const & cInKey)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());
	pcImpl->m_aPaths.push_back(cInKey);
	return *this;
}

void H3DF::KeyPath::ShowString(CString & strOutPath)
{
	auto pcImpl = static_cast<KeyPathImpl *>(m_pcImpl.get());

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