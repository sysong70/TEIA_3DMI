#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Object.h"

#include "../3DF.MetaData.h"

namespace H3DF
{
	class MetaDataImpl : public Impl
	{
	public:
		MetaDataImpl();
		void Copy(MetaDataImpl * pcInThat);

		MetaDataIndex m_eIndex = MetaDataIndex::None;
	};

	class StringMetaDataImpl : public MetaDataImpl
	{
	public:
		StringMetaDataImpl();
		void Copy(StringMetaDataImpl * pcInThat);

		CString m_strValue;
	};

	class DwordPtrMetaDataImpl : public MetaDataImpl
	{
	public:
		DwordPtrMetaDataImpl();
		void Copy(DwordPtrMetaDataImpl * pcInThat);

		DWORD_PTR m_nValue = 0;
	};
}