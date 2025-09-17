#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Geometry.h"

#include "Color.h"

#include <atlcoll.h>

namespace H3DF
{
    class API_3DF TextKey : public GeometryKey {
    public:
        TextKey();
        explicit TextKey(Key const & in_that);
        TextKey(TextKey const & in_that);

        TextKey & operator = (TextKey const & other);
    };
}