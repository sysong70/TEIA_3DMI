#pragma once

#include "../../3DF/Impl/3DF.Impl.h"
#include "../../3DF/Object.h"

#include "../3DF.MetaData.h"

namespace H3DF
{
	class MetaDataImpl : public Impl
	{
	public:
		MetaDataImpl();

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<MetaDataImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const MetaDataImpl * pcInThat) {
			m_eIndex = pcInThat->m_eIndex;
		}

		bool Equals(const MetaDataImpl * pcInThat) const {
			return m_eIndex == pcInThat->m_eIndex;
		}

		MetaDataIndex m_eIndex = MetaDataIndex::None;
	};

	class StringMetaDataImpl : public MetaDataImpl
	{
	public:
		StringMetaDataImpl();

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<StringMetaDataImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const StringMetaDataImpl * pcInThat) {
			MetaDataImpl::Copy(pcInThat);
			m_strValue = pcInThat->m_strValue;
		}

		bool Equals(const StringMetaDataImpl * pcInThat) const {
			return (m_strValue == pcInThat->m_strValue && MetaDataImpl::Equals(pcInThat));
		}

		CString m_strValue;
	};

	class DwordPtrMetaDataImpl : public MetaDataImpl
	{
	public:
		DwordPtrMetaDataImpl();

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<DwordPtrMetaDataImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const DwordPtrMetaDataImpl * pcInThat) {
			MetaDataImpl::Copy(pcInThat);
			m_nValue = pcInThat->m_nValue;
		}

		bool Equals(const DwordPtrMetaDataImpl * pcInThat) const {
			return (m_nValue == pcInThat->m_nValue && MetaDataImpl::Equals(pcInThat));
		}

		DWORD_PTR m_nValue = 0;
	};
}