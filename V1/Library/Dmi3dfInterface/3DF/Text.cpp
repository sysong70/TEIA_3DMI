#include "StdAfx.h"

#include "Text.h"

#include "./Impl/GeometryImpl.h"

#include "3DF.Utility.h"

#include <hc.h>
#include <HUtility.h>
#include <HTools.h>

#include <format>
#include <locale>
#include <codecvt>

using namespace H3DF;

namespace H3DF
{
	class TextKeyImpl : public GeometryKeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<TextKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy (const Impl * pcInThat) override
		{
			if (pcInThat == nullptr) {
				DEBUG_RETURN;
			}

			GeometryKeyImpl::Copy(pcInThat);

			auto pcImpl = static_cast<const TextKeyImpl *>(pcInThat);
		}
	};
}

H3DF::TextKey::TextKey()
{
	INIT_IMPL(TextKey);
}

H3DF::TextKey::TextKey(Key const & cInThat)
{
	INIT_IMPL(TextKey);

	auto pcInKeyImpl = static_cast<const KeyImpl *>(cInThat.GetImpl());

	// 명시적 캐스팅을 해야 합니다.
	static_cast<KeyImpl *>(m_pcImpl.get())->Copy(pcInKeyImpl);
}

H3DF::TextKey::TextKey(TextKey const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

TextKey & H3DF::TextKey::operator=(TextKey const & cInOther)
{
	if (nullptr != cInOther.m_pcImpl) {
		m_pcImpl = cInOther.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

TextKey & H3DF::TextKey::SetPosition(Point const & position)
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return *this;
	}

	HC_Move_Text(key, position.x, position.y, position.z);

	return *this;
}

TextKey & H3DF::TextKey::SetText(std::wstring_view text)
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if(INVALID_KEY == key) {
		DEBUG_STOP;
		return *this;
	}

	std::string convText = H3DF::Utility::WStringToUtf8(std::wstring(text));

	int count = 0;
	HC_Show_Text_Count(key, &count);

	HC_Edit_Text(key, 0, 0, 0, count, convText.c_str());

	return *this;
}

TextKey & H3DF::TextKey::SetColor(RGBAColor const & color)
{

	return *this;
}

TextKey & H3DF::TextKey::SetBold(bool enabled)
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return *this;
	}

	HC_Open_Geometry(key); {
		std::string list = std::format("bold={}", enabled ? "on" : "off");
		HC_Set_Text_Font(list.c_str());
	} HC_Close_Geometry();

	return *this;
}

/*
TextKey & H3DF::TextKey::SetBackground(bool bInState, CStringA strName)
{
	return *this;
}

TextKey & H3DF::TextKey::SetBackground(CStringA strName)
{
	return *this;
}

TextKey & H3DF::TextKey::SetBackground(bool bInState)
{
	return *this;
}

bool H3DF::TextKey::ShowBackground(bool & bOutState, CString & cOutName) const
{
	//HC_Show_Text()

	return false;
}

*/

TextKey & H3DF::TextKey::SetSize(float size, Text::SizeUnits units)
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return *this;
	}
	
	std::string list;

	switch (units)
	{
	case H3DF::Text::SizeUnits::ObjectSpace:
		list = std::format("size={} oru", size);
		break;

	case H3DF::Text::SizeUnits::SubscreenRelative:
		list = std::format("size={} sru", size);
		break;

	case H3DF::Text::SizeUnits::WindowRelative:
		list = std::format("size={} wru", size);
		break;

	case H3DF::Text::SizeUnits::WorldSpace:
		list = std::format("size={} wsu", size);
		break;

	case H3DF::Text::SizeUnits::Points:
		list = std::format("size={} points", size);
		break;

	case H3DF::Text::SizeUnits::Pixels:
		list = std::format("size={} pixels", size);
		break;
	}

	HC_Open_Geometry(key); {
		HC_Set_Text_Font(list.c_str());
	} HC_Close_Geometry();
	return *this;
}

TextKey & H3DF::TextKey::SetFont(std::string_view name)
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return *this;
	}

	HC_Open_Geometry(key); {
		std::string list = std::format("name={}", name);
		HC_Set_Text_Font(list.c_str());
	} HC_Close_Geometry();

	return *this;
}

bool H3DF::TextKey::ShowPosition(Point & position) const
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return false;
	}

	int count = 0;
	HC_Show_Text_Count(key, &count);

	if (0 == count) {
		return false;
	}

	float x, y, z;
	char * textBuffer = new char[count + 1];

	std::string outText;

	HC_Show_Text(key, &x, &y, &z, textBuffer);

	position.x = x;
	position.y = y;
	position.z = z;

	delete[] textBuffer;

	return true;
}

bool H3DF::TextKey::ShowText(std::wstring & text) const
{
	IMPL(TextKey);

	HC_KEY key = impl->KeyValue();
	if (INVALID_KEY == key) {
		DEBUG_STOP;
		return false;
	}

	int count = 0;
	HC_Show_Text_Count(key, &count);

	if (0 == count) {
		return false;
	}

	float x, y, z;
	char * textBuffer = new char[count + 1];

	std::string outText;

	HC_Show_Text(key, &x, &y, &z, textBuffer);

	// UTF-8 -> std::wstring
	std::string utf8_text = textBuffer;
	text = H3DF::Utility::Utf8ToWString(utf8_text);

	delete[] textBuffer;

	return true;
}