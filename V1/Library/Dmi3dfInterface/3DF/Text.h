#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Geometry.h"

#include "Color.h"

#include <string>

namespace H3DF
{
    class API_3DF TextKey : public GeometryKey {
    public:
        TextKey();
        explicit TextKey(Key const & source);
        TextKey(TextKey const & other);

        TextKey & operator = (TextKey const & other);

        TextKey & SetPosition(Point const & position);

        TextKey & SetText(std::wstring_view text);
 
        TextKey & SetColor(RGBAColor const & color);

        TextKey & SetBold(bool enabled);

        TextKey & SetSize(float size, Text::SizeUnits units);

        TextKey & SetFont(std::string_view name);

		TextKey & SetBackground(bool enabled, std::string_view name);
		TextKey & SetBackground(std::string_view name);
		TextKey & SetBackground(bool enabled);

        bool ShowPosition(Point & position) const;
        bool ShowText(std::wstring & text) const;
       

//        bool ShowBackground(bool & enabled, std::string & outName) const;
    };
}