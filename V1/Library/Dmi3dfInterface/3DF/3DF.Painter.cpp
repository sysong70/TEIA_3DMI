#include "stdafx.h"
#include "3DF.Painter.h"
#include "HBaseView.h"



#pragma region Internal

namespace TDF
{
    namespace Painter
    {
        HBaseView* View = nullptr;



        class Format
        {
        private:

            CStringA buffer;

        public:

            Format(const char* format, ...)
            {
                va_list ap;

                va_start(ap, format);
                buffer.FormatV(format, ap);
                va_end(ap);
            }

            operator char const* () const
            {
                return buffer;
            }
        };

        class FormatW
        {
        private:

            CString buffer;

        public:

            FormatW(const wchar_t* format, ...)
            {
                va_list ap;

                va_start(ap, format);
                buffer.FormatV(format, ap);
                va_end(ap);
            }

            operator wchar_t const* () const
            {
                return buffer;
            }
        };



        bool CheckOption(const char* option, ...)
        {
            if (option == nullptr) {
                return true;
            }

            static char buffer[2048] = { '\0' };
            const char* arg;
            bool found = false;

            va_list ap;
            va_start(ap, option);

            while (true) {
                arg = va_arg(ap, const char*);
                if (arg == nullptr) {
                    break;
                }

                if (::strcmp(option, arg) == 0) {
                    found = true;
                    break;
                }
            }

            va_end(ap);

            return found;
        }
    }
}

#pragma endregion //:REGION

#pragma region Root

void TDF::Painter::SetView(HBaseView* view)
{
    View = view;
}



void TDF::Painter::SetColor(const char* type, const char* space, double abc[3])
{
    ASSERT(CheckOption(type,
        // If there are no light sources in a given scene, the "ambient" light color is ignored.
        // If there is at least one light, HOOPS will do a full lighting calculation and the "ambient" color will define a diffuse background lighting coming from all directions.
        // You would usually use a combination of specific light sources plus ambient light.
        // Ambient light applies to all geometry within its scope in the segment tree.
        // It is usually specified at the window level.
        "ambient",
        // A synonym for "ambient".
        "ambient light",
        // Possible only when a polygon handedness is given and backplane culls are turned off, applies to the backplane of any face.
        "back",
        // The straight lines connecting the vertices of a polygon, shell, or mesh, or the border of an ellipse or circle.
        "edges",
        // Used when a contrasting color is required for drawing pat terned faces.
        "face contrast",
        // The interior filled areas of polygons, shells, meshes, ellipses, and circles.
        "faces",
        // Shorthand for faces and edges (i.e., surfaces), markers, polylines, and text all at once.
        "geometry",
        // A synonym for "lights".
        "lighting",
        // Used for any light sources within its scope.
        // Does not affect the color of a light located elsewhere in the segment tree but shining into this part of the tree.
        // The "lightness" of the color controls the brightness of the light.
        "lights",
        // Synonymous with "polylines".
        "lines",
        // Applies to marker symbols.
        "markers",
        // A synonym for "face contrast". "Face contrast" is preferred.
        "polygon contrast",
        // Shorthand for both faces and edges.
        "polygons",
        // Applies to lines, polylines, and inked lines.
        "polylines",
        // Applies to text strings.
        "text",
        // Used when window backgrounds require a contrasting color, either for drawing window frames or for drawing window pat terns.
        "window contrast",
        // Used for window backgrounds.
        "windows",
        NULL
    ));

    ASSERT(CheckOption(space,
        // For hue, lightness, and saturation (a "double cone", with black and white at the tips).
        "HLS",
        // For hue, lightness, and value (a "single cone" standing on its point, with black at the tip and white at the center of the base).
        "HSV",
        // For hue, intensity, and chromaticity (a cylinder, with black and white at the two ends)
        "HIC",
        // For red, green, and blue (a cube, with white, black, and six primary colors at the corners).
        "RGB"
    ));

    HC_Set_Color_By_Value(type, space, abc[0], abc[1], abc[2]);
}

TDF::Point TDF::Painter::TransColor(COLORREF color)
{
    TDF::Point rgb;

    rgb.x = GetRValue(color) / 255.0;
    rgb.y = GetGValue(color) / 255.0;
    rgb.z = GetBValue(color) / 255.0;

    return rgb;
}

#pragma endregion //:REGION

#pragma region Arc

HC_KEY TDF::Painter::Arc::Create(TDF::Point first, TDF::Point second, TDF::Point third)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circular_Arc(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}



void TDF::Painter::Arc::GetPoints(TDF::Point center, double radius, double startAngle, double endAngle, Points& points)
{
    if (endAngle < startAngle) {
        endAngle += 360;
    }

    int step = endAngle - startAngle;

    for (int i = (int)startAngle; i <= (int)endAngle; i++) {
        double angle = i * 3.141592 / 180;
        TDF::Point p(cos(angle), sin(angle));
        points.push_back(center + (p * radius));
    }
}



void TDF::Painter::Arc::GetPoints(float x, float y, double radius, double startAngle, double endAngle, Points& points)
{
    GetPoints(TDF::Point(x, y), radius, startAngle, endAngle, points);
}

#pragma endregion //:REGION

#pragma region Circle

HC_KEY TDF::Painter::Circle::Create(TDF::Point center, double radius, bool polygon)
{
    center.z = 0;

    if (polygon) {
        Points points;

        for (int i = 0; i <= 360; i += 6) {
            double angle = i * 3.141592 / 180;
            TDF::Point p(cos(angle), sin(angle));
            points.push_back(center + (p * radius));
        }

        return Polygon::Create(points);
    }
    else {
        HC_KEY key = HC_Insert_Circle_By_Radius(&center, radius, NULL);
        ASSERT(key != HC_ERROR_KEY);

        return key;
    }
}



HC_KEY TDF::Painter::Circle::Create(TDF::Point first, TDF::Point second, TDF::Point third, bool polygon)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circle(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}



void TDF::Painter::Circle::GetPoints(TDF::Point center, double radius, bool reverse, Points& points)
{
    if (reverse) {
        for (int i = 360; i >= 0; i -= 6) {
            double angle = i * 3.141592 / 180;
            TDF::Point p(cos(angle), sin(angle));
            points.push_back(center + (p * radius));
        }
    }
    else {
        for (int i = 0; i <= 360; i += 6) {
            double angle = i * 3.141592 / 180;
            TDF::Point p(cos(angle), sin(angle));
            points.push_back(center + (p * radius));
        }
    }
}

#pragma endregion //:REGION

#pragma region Compute

double TDF::Painter::Compute::Distance(TDF::Point p1, TDF::Point p2)
{
    TDF::Point d = p2 - p1;
    return sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
}



double TDF::Painter::Compute::PixelToWorld(double value)
{
    Point points[2];
    points[1].x = value;

    HC_Open_Segment_By_Key(View->GetSceneKey());
    HC_Compute_Coordinates(".", "local pixels", &points[0], "world", &points[0]);
    HC_Compute_Coordinates(".", "local pixels", &points[1], "world", &points[1]);
    HC_Close_Segment();

    return Distance(points[0], points[1]);
}



TDF::Vector TDF::Painter::Compute::Normal()
{
    Vector normal;

    HC_Open_Segment_By_Key(View->GetSceneKey());
    {
        TDF::Point position;
        HC_Show_Net_Camera_Position(&position.x, &position.y, &position.z);

        TDF::Point target;
        HC_Show_Net_Camera_Target(&target.x, &target.y, &target.z);

        normal = target - position;
        HC_Compute_Normalized_Vector(&normal, &normal);
    }
    HC_Close_Segment();

    return normal;

}

#pragma endregion //:REGION

#pragma region Font

void TDF::Painter::Font::SetAlignment(const char* value)
{
    // value is 1 or 2 char
    // ^ top
    // v bottom(base)
    // * center
    // < left
    // > right
    HC_Set_Text_Alignment(value);
}

void TDF::Painter::Font::SetAlignment(EPivot value)
{
    switch (value) {
    case EPivot::TopLeft:       HC_Set_Text_Alignment("^<"); break;
    case EPivot::TopCenter:     HC_Set_Text_Alignment("^*"); break;
    case EPivot::TopRight:      HC_Set_Text_Alignment("^>"); break;
    case EPivot::MiddleLeft:    HC_Set_Text_Alignment("*<"); break;
    case EPivot::MiddleCenter:  HC_Set_Text_Alignment("**"); break;
    case EPivot::MiddleRight:   HC_Set_Text_Alignment("*>"); break;
    case EPivot::BottomLeft:    HC_Set_Text_Alignment("v<"); break;
    case EPivot::BottomCenter:  HC_Set_Text_Alignment("v*"); break;
    case EPivot::BottomRight:   HC_Set_Text_Alignment("v>"); break;
    default:
        break;
    }
}



void TDF::Painter::Font::SetBold(bool value)
{
    HC_Set_Text_Font(Format("bold = %s", (value ? "on" : "off")));
}



void TDF::Painter::Font::SetName(const char* value)
{
    HC_Set_Text_Font(Format("name = %s", value));
}



void TDF::Painter::Font::SetSize(double size, const char* unit)
{
    HC_Set_Text_Font(Format("size = %.6f %s", size, unit));
}



void TDF::Painter::Font::SetRenderer(const char* value)
{
    HC_Set_Text_Font(Format("renderer = %s", value));
}



void TDF::Painter::Font::SetRotation(double degree)
{
    if (fabs(degree) < 0.000001) {
        HC_Set_Text_Font("no ratation");
    }
    else {
        HC_Set_Text_Font(Format("rotation = %3d", degree));
    }
}



void TDF::Painter::Font::SetTransform(bool value)
{
    HC_Set_Text_Font(Format("transforms = %s", (value ? "on" : "off")));
}

void TDF::Painter::Font::SetTransform(const char* value)
{
    ASSERT(CheckOption(value,
        // Transforms” (or “transforms = on”) tells the system to pick a font that can and should be subject to transformations just like regular geometry.
        // In other words, a “transformable” font is subject to scaling, rotations, perspective transformations, etc.
        // Setting “transforms” causes the Text Path to be added onto all of the other HOOPS transformations.
        "on",
        // “No transforms” (or “transforms = off”) says that only the starting position of the text (as determined by the Text Alignment) is affected by the camera, the modelling, etc.
        // After that, the text is drawn in the screen coordinate system.
        // Text Path works relative to the screen - a (0, 1, 0) path runs straight up the screen.
        "off",
        // Since fully-transformable text commonly requires falling back to the stroked font, you can specify “transforms= character position only” or “transforms= character position adjusted”.
        // In both cases, all the text transforms involved in positioning the characters will be performed, but the characters themselves will be drawn erect and facing forwards.
        "character position only",
        "character position adjusted",
        NULL
    ));

    HC_Set_Text_Font(Format("transforms = %s", value));
}

#pragma endregion //:REGION

#pragma region Figure

HC_KEY TDF::Painter::Figure::CreateDonut(TDF::Point center, double inner, double outer)
{
    Points points;

    Circle::GetPoints(center, inner, false, points);
    Circle::GetPoints(center, outer, true, points);

    return Polygon::Create(points);
}



HC_KEY TDF::Painter::Figure::CreateObround(TDF::Point topLeft, TDF::Point bottomRight)
{
    Points points;
    TDF::Point center = (bottomRight + topLeft) / 2;
    double dist = fabs(topLeft.y - bottomRight.y) / 2;

    Arc::GetPoints(topLeft.x, center.y, dist, 90, 270, points);
    Arc::GetPoints(bottomRight.x, center.y, dist, 270, 90, points);

    return Polygon::Create(points);
}



HC_KEY TDF::Painter::Figure::CreateRectangle(TDF::Point topLeft, TDF::Point bottomRight)
{
    topLeft.z = 0;
    bottomRight.z = 0;

    TDF::Point points[5];

    points[0] = topLeft;
    points[1].x = bottomRight.x; points[1].y = topLeft.y;
    points[2] = bottomRight;
    points[3].x = topLeft.x; points[3].y = bottomRight.y;
    points[4] = topLeft;

    HC_KEY key = HC_Insert_Polygon(5, points);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}

#pragma endregion //:REGION

#pragma region Line

HC_KEY TDF::Painter::Line::Create(TDF::Point first, TDF::Point second, bool firstEnd, bool secondEnd)
{
    HC_KEY key = HC_Insert_Line(first.x, first.y, 0, second.x, second.y, 0);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}

#pragma endregion //:REGION

#pragma region Polyline

HC_KEY TDF::Painter::Polyline::Create(Points& points)
{
    HC_KEY key = HC_Insert_Polyline(points.size(), points.data());
    ASSERT(key != HC_ERROR_KEY);

    return key;
}

#pragma endregion //:REGION

#pragma region Polygon

HC_KEY TDF::Painter::Polygon::Create(Points& points)
{
    HC_KEY key = HC_Insert_Polygon(points.size(), points.data());
    ASSERT(key != HC_ERROR_KEY);

    return key;
}

#pragma endregion //:REGION

#pragma region Segment

void TDF::Painter::Segment::SetColor(const char* type, COLORREF color)
{
    double rgb[3];
    rgb[0] = GetRValue(color) / 255.0;
    rgb[1] = GetGValue(color) / 255.0;
    rgb[2] = GetBValue(color) / 255.0;

    HC_Set_Color_By_Value(type, "RGB", rgb[0], rgb[1], rgb[2]);
}

void TDF::Painter::Segment::SetColor(const char* type, COLORREF color, double alpha)
{
    double rgb[3];
    rgb[0] = GetRValue(color) / 255.0;
    rgb[1] = GetGValue(color) / 255.0;
    rgb[2] = GetBValue(color) / 255.0;

    HC_Set_Color(Format("%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))",
        type, rgb[0], rgb[1], rgb[2], alpha, alpha, alpha));
}



void TDF::Painter::Segment::SetEdgePattern(const char* value)
{
    ASSERT(CheckOption(value,
        "---",          // A solid line.
        "- -",          // A simple dashed line.
        "....",         // A dotted line.
        "-.-.",         // Dashes and dots alternating.
        "-..-..",       // Dashes and double-dot alternating.
        "-...",         // Dashes and triple-dot alternating.
        "---- ----",    // Long dashes.
        "center",       // Very-long-dash and short-dash alternating.
        "phantom",      // Very-long-dash and double-short-dash alternating.
        NULL
    ));

    HC_Set_Edge_Pattern(value);
}



void TDF::Painter::Segment::SetEdgeWeight(double value)
{
    HC_Set_Edge_Weight(value);
}



void TDF::Painter::Segment::SetEdgeType(double weight, const char* pattern, COLORREF color)
{
    SetEdgeWeight(weight);
    if (pattern != nullptr) {
        SetEdgePattern(pattern);
    }
    if (color != 0) {
        SetColor("edges", color);
    }
}



void TDF::Painter::Segment::SetLinePattern(const char* value, const char* prefix, const char* suffix)
{
    ASSERT(CheckOption(value,
        "---",      // A solid line.        
        "- -",      // A simple dashed line.
        "...",      // A dotted line.
        "-.",       // Dashes and dots alternating.
        "-..",      // Dashes and double-dot alternating.
        "-...",     // Dashes and triple-dot alternating.
        "-- --",    // Long dashes.
        "center",   // Very-long-dash and shot dash alternating.
        "phantom",  // Very-long-dash and double-shot-dash alternating.
        "...fine",  // A very fine dotted line.
        "finedot",  // Same as "...fine"
        NULL
    ));

    // Prefix - 1 or 2
    // "|", A "butt" line cap.
    // "[", A "square" line cap.
    // "(", A "round" line cap.
    // "<", A "mitre" line cap

    // Suffix - 1 or 2
    // "#", A "square" segment cap.
    // "^", A "mitre" segment cap.
    // "@", A "round" line segment cap.
    // ">", A "mitre" line join.
    // "]", A "bevel" line join.
    // ")", A "round" line join.

    //:CHECK
    HC_Set_Line_Pattern(value);
}



void TDF::Painter::Segment::SetLineWeight(double value)
{
    HC_Set_Line_Weight(value);
}



void TDF::Painter::Segment::SetLineType(double weight, const char* pattern, COLORREF color)
{
    SetLineWeight(weight);
    if (pattern != nullptr) {
        SetLinePattern(pattern);
    }
    if (color != 0) {
        SetColor("lines", color);
    }
}



void TDF::Painter::Segment::SetVisibility(const char* type, bool value)
{
    HC_Set_Visibility(Format("%s = %s", type, (value ? "on" : "off")));
}

void TDF::Painter::Segment::SetVisibility(const char* option, const char* sub, bool value)
{
    ASSERT(CheckOption(option,
        "cutting plane",
        "cut edges",
        "cut geometry",
        "edges",
        "faces",
        "geometry",
        "images",
        "lighting",
        "lights",
        "lines",
        "vertices",
        "markers",
        "everything",
        "polygons",
        "polylines",
        "shadows",
        "string cursors",
        "text",
        "windows",
        NULL
    ));

    ASSERT(CheckOption(sub,
        // special edge options
        "adjacent",
        "adjacent only", // null value
        "generics",
        "hard",
        "interior silhouettes",
        "interior silhouettes only", // null value
        "mesh quads",
        "mesh quads only", // null value
        "nonculled",
        "perimeters",
        "perimeters only" // null value
        // shadows
        "casting",
        "receiving",
        "emitting",
        NULL
    ));

    if (sub == nullptr) {
        HC_Set_Visibility(Format("%s = %s", option, (value ? "on" : "off")));
    }
    else if (sub == "adjacent only" ||
        sub == "interior silhouettes only" ||
        sub == "mesh quads only" ||
        sub == "perimeters only") {
        HC_Set_Visibility(Format("%s = (%s)", option, sub));
    }
    else if (CStringA(sub).IsEmpty() == false) {
        HC_Set_Visibility(Format("%s = (%s = %s)", option, sub, (value ? "on" : "off")));
    }
    else {
        ASSERT(FALSE);
    }
}

#pragma endregion //:REGION

#pragma region Text

HC_KEY TDF::Painter::Text::Create(TDF::Point center, const char* value)
{
    HC_KEY key = HC_Insert_Text(center.x, center.y, center.z, value);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}

HC_KEY TDF::Painter::Text::Create(TDF::Point center, const wchar_t* value)
{
    HC_KEY key = HC_Insert_Text_With_Encoding(center.x, center.y, center.z, "wcs", value);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}



void TDF::Painter::Text::GetExtent(const char* value, float& width, float& height)
{
    HC_Compute_Text_Extent(".", value, &width, &height);
}

void TDF::Painter::Text::GetExtent(const wchar_t* value, float& width, float& height)
{
    HC_Compute_Text_Extent_With_Encoding(".", "utf16", value, &width, &height);
}



void TDF::Painter::Text::Update(HC_KEY key, const wchar_t* value)
{
    //:TODO
    ASSERT(FALSE);
}

#pragma endregion //:REGION

#pragma region Cursor

HC_KEY TDF::Painter::Cursor::Create(HC_KEY textKey, int row, int column)
{
    HC_KEY key = HC_Insert_String_Cursor(textKey, row, column);
    ASSERT(key != HC_ERROR_KEY);

    return key;
}



void TDF::Painter::Cursor::Hide(HC_KEY key)
{
    HC_Flush_By_Key(key);
}



void TDF::Painter::Cursor::Move(HC_KEY key, int row, int column)
{
    HC_Move_String_Cursor(key, row, column);
}

#pragma endregion //:REGION