#pragma once

#include "../3DF/3DF.h"
#include "Sprocket.h"

namespace H3DF
{
	class API_3DF MetaData : public Sprocket
	{
	public:
		MetaData();
		MetaData(const MetaData & cInThat);
		MetaData(MetaDataIndex nInIndex);

		MetaData & operator = (MetaData const & cInThat);

		bool Equals(MetaData const & cInThat) const;
		bool operator != (MetaData const & cInThat) const;
		bool operator == (MetaData const & cInThat) const;

		MetaDataIndex GetIndex() const;
		void SetIndex(MetaDataIndex nInIndex);
	};

	class API_3DF StringMetaData : public MetaData {
	public:
		StringMetaData();
		StringMetaData(MetaData const & cInThat);
		StringMetaData(StringMetaData const & cInThat);
		StringMetaData(MetaDataIndex nInIndex, CString strInValue);

		StringMetaData & operator = (StringMetaData const & cInThat);

		bool operator != (StringMetaData const & cInThat) const;
		bool operator == (StringMetaData const & cInThat) const;

		CString GetValue() const;
		void SetValue(CString strInValue);
	};

	class API_3DF DwordPtrMetaData : public MetaData {
	public:
		DwordPtrMetaData();
		DwordPtrMetaData(MetaData const & cInThat);
		DwordPtrMetaData(DwordPtrMetaData const & cInThat);
		DwordPtrMetaData(MetaDataIndex nInIndex, DWORD_PTR nInValue);

		DwordPtrMetaData & operator = (DwordPtrMetaData const & cInThat);

		bool operator != (DwordPtrMetaData const & cInThat) const;
		bool operator == (DwordPtrMetaData const & cInThat) const;

		DWORD_PTR GetValue() const;
		void SetValue(DWORD_PTR nInValue);
	};
}

