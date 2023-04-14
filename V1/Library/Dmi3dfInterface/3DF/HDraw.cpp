#include "stdafx.h"
#include "HDraw.h"
#include "HBaseView.h"

USING_3DF_NAMESPACE

namespace HDraw
{
    HBaseView* View = nullptr;
}


void HDraw::Test(HBaseView* view, TDF::Math::Matrix & cMatrix, HPoint p1, HPoint p2, Point op1, Point op2)
{
    SetView(view);

    HC_Open_Segment("construct");
    {
        // Remove previous

        HC_Flush_Contents(".", "geometry, segment");

        // Set default settings

        HC_Set_Color("edges = white");
        HC_Set_Color("faces = black");
        HC_Set_Color("lines = white");
        HC_Set_Color("text = white");

        HC_Set_Visibility("edges");
        HC_Set_Visibility("faces");
        HC_Set_Visibility("lines");
        HC_Set_Visibility("text");

        // Draw line

        HC_Open_Segment("world");
        {
            HC_Set_Line_Weight(2);
            HC_Set_Line_Pattern("- -");

            Line::Create(p1, p2);
        }
        HC_Close_Segment();

        // Draw first symbol

        HC_Open_Segment("first");
        {
			HC_Set_Color("edges = black");
			HC_Set_Color("faces = white");

            HC_Set_Edge_Weight(4);
            //:TODO - calculate point or use segment metrix
            double radius = Compute::PixelToWorld(8);
            Circle::Create(p1, radius, false);
        }
        HC_Close_Segment();

        // Draw second symbol

        HC_Open_Segment("second");
        {
			HC_Set_Color("edges = black");
			HC_Set_Color("faces = white");

            HC_Set_Edge_Weight(4);
            //:TODO - calculate point or use segment metrix
            double radius = Compute::PixelToWorld(8);
            Circle::Create(p2, radius, false);
        }
        HC_Close_Segment();

        // Draw text and outer frame

		HPoint center = (p1 + p2) / 2.0f;

        Point cP1(p1.x, p1.y, p1.z);
        Point cP2(p2.x, p2.y, p2.z);;

        Vector cXAixs = cMatrix.XAxis();
        Vector cYAixs = cMatrix.YAxis();
        Vector cZAixs = cMatrix.ZAxis();
        Vector cOrigin = cMatrix.Origin();

		Point2D cDrop1 = op1.DropPoint(cOrigin, cXAixs, cYAixs);
		Point2D cDrop2 = op2.DropPoint(cOrigin, cXAixs, cYAixs);

/*
		HC_Open_Segment("test"); {
			HC_Set_Line_Weight(2);
            HC_Set_Color("geometry=blue");
            HC_Insert_Line(0.0, 0.0, 0.0, 100, 0, 0);
            HC_Insert_Line(0.0, 0.0, 0.0, 0, 100, 0);

            HC_Insert_Line(0.0, 0.0, 0.0, cP1.x, cP1.y, cP1.z);
            HC_Insert_Line(0.0, 0.0, 0.0, cP2.x, cP2.y, cP2.z);
		} HC_Close_Segment();

		HC_Open_Segment("test1"); {
			HC_Set_Line_Weight(2);
            HC_Set_Color("geometry=red");
            HC_Insert_Line(cDrop1.x, cDrop1.y, 0.0, cDrop2.x, cDrop2.y, 0.0);
		} HC_Close_Segment();
*/


        Vector2D cDropVector = cDrop2 - cDrop1;

        Vector cVector(cDropVector);

        //cZAixs = -cZAixs;

        float fAngle = Vector::ZAxis().CCWAngleWith(Vector::XAxis(), cVector);
        //float fAngle = Vector::XAxis().CCWAngleWith(cVector);

        TRACE(L"TextAngle: %f\n", fAngle);

//         if (180.0f > fAngle) {
//             fAngle = fAngle - 180.0f;
//         }

		TDF::Math::Matrix cRotation;
        cRotation.RotateOffAxis(TDF::Vector::ZAxis(), fAngle);
        cRotation.Translate(center.x, center.y, center.z);
		
        HC_Open_Segment("text");
        {
            //HC_Rotate_Object(0, 0, 10);
            HC_Set_Modelling_Matrix(cRotation.m_fData);

            HC_Set_Edge_Weight(2);

            Font::SetName("arial");
            Font::SetBold();
            Font::SetSize(Compute::PixelToWorld(32), "oru");
            Font::SetRenderer("truetype");
            Font::SetTransform();

            Format value("%.3f mm", Compute::Distance(p1, p2));
            float width, height;
            Text::GetExtent(value, width, height);

            //:TODO - calculate point or use segment metrix
            
			HPoint cTextCenter = center;
			cTextCenter.y += -height / 2.0f;

			//Text::Create(cTextCenter, value);
            Text::Create(HPoint(0, -height / 2), value);

			double offset = Compute::PixelToWorld(8);
			HPoint cFigureCenter = center;
			HPoint cOffset1(-width / 2, height / 2 + offset);
			HPoint cOffset2(width / 2, -height / 2 - offset);

			//:TODO - calculate point or use segment metrix
			//Figure::CreateObround(cFigureCenter + cOffset1, cFigureCenter + cOffset2);
			Figure::CreateObround(HPoint(-width / 2, height / 2 + offset), HPoint(width / 2, -height / 2 - offset));
        }
        HC_Close_Segment();
    }
    HC_Close_Segment();

    SetView(nullptr);
}



void HDraw::SetView(HBaseView* view)
{
    View = view;
}



void HDraw::Arc::Create(HPoint first, HPoint second, HPoint third)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circular_Arc(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);
}

void HDraw::Arc::GetPoints(HPoint center, double radius, double startAngle, double endAngle, HPoints& points)
{
    if (endAngle < startAngle) {
        endAngle += 360;
    }

    int step = endAngle - startAngle;

    for (int i = (int)startAngle; i <= (int)endAngle; i++) {
        double angle = i * 3.141592 / 180;
        HPoint p(cos(angle), sin(angle));
        points.push_back(center + (p * radius));
    }
}

void HDraw::Arc::GetPoints(float x, float y, double radius, double startAngle, double endAngle, HPoints& points)
{
    GetPoints(HPoint(x, y), radius, startAngle, endAngle, points);
}



void HDraw::Circle::Create(HPoint center, double radius, bool polygon)
{
    center.z = 0;

    if (polygon) {
        HPoints points;

        for (int i = 0; i <= 360; i += 6) {
            double angle = i * 3.141592 / 180;
            HPoint p(cos(angle), sin(angle));
            points.push_back(center + (p * radius));
        }

        Polygon::Create(points);
    }
    else {
        HC_KEY key = HC_Insert_Circle_By_Radius(&center, radius, NULL);
        ASSERT(key != HC_ERROR_KEY);
    }
}

void HDraw::Circle::Create(HPoint first, HPoint second, HPoint third, bool polygon)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circle(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);
}



double HDraw::Compute::Distance(HPoint p1, HPoint p2)
{
    HPoint d = p2 - p1;
    return sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
}

double HDraw::Compute::PixelToWorld(double value)
{
    HVector points[2];
    points[1].x = value;

    HC_Open_Segment_By_Key(View->GetSceneKey());
    HC_Compute_Coordinates(".", "local pixels", &points[0], "world", &points[0]);
    HC_Compute_Coordinates(".", "local pixels", &points[1], "world", &points[1]);
    HC_Close_Segment();

    return Distance(points[0], points[1]);
}

HVector HDraw::Compute::Normal()
{
    HVector normal;

    HC_Open_Segment_By_Key(View->GetSceneKey());
    {
        HPoint position;
        HC_Show_Net_Camera_Position(&position.x, &position.y, &position.z);

        HPoint target;
        HC_Show_Net_Camera_Target(&target.x, &target.y, &target.z);

        normal = target - position;
        HC_Compute_Normalized_Vector(&normal, &normal);
    }
    HC_Close_Segment();

    return normal;

}



void HDraw::Font::SetBold(bool value)
{
    HC_Set_Text_Font(Format("bold = %s", (value ? "on" : "off")));
}

void HDraw::Font::SetName(const char* value)
{
    HC_Set_Text_Font(Format("name = %s", value));
}

void HDraw::Font::SetSize(double size, const char* unit)
{
    HC_Set_Text_Font(Format("size = %.3f %s", size, unit));
}

void HDraw::Font::SetRenderer(const char* value)
{
    HC_Set_Text_Font(Format("renderer = %s", value));
}

void HDraw::Font::SetTransform(bool value)
{
    HC_Set_Text_Font(Format("transforms = %s", (value ? "on" : "off")));
}



void HDraw::Figure::CreateObround(HPoint topLeft, HPoint bottomRight)
{
    HPoints points;
    HPoint center = (bottomRight + topLeft) / 2;
    double dist = fabs(topLeft.y - bottomRight.y) / 2;

    Arc::GetPoints(topLeft.x, center.y, dist, 90, 270, points);
    Arc::GetPoints(bottomRight.x, center.y, dist, 270, 90, points);

    Polygon::Create(points);
}



void HDraw::Line::Create(HPoint first, HPoint second, bool firstEnd, bool secondEnd)
{
    HC_KEY key = HC_Insert_Line(first.x, first.y, 0, second.x, second.y, 0);
    ASSERT(key != HC_ERROR_KEY);
}



void HDraw::Polyline::Create(HPoints& points)
{
    HC_KEY key = HC_Insert_Polyline(points.size(), points.data());
    ASSERT(key != HC_ERROR_KEY);
}



void HDraw::Polygon::Create(HPoints& points)
{
    HC_KEY key = HC_Insert_Polygon(points.size(), points.data());
    ASSERT(key != HC_ERROR_KEY);
}



void HDraw::Text::Create(HPoint center, const char* value)
{
    HC_KEY key = HC_Insert_Text(center.x, center.y, 0, value);
    ASSERT(key != HC_ERROR_KEY);
}

void HDraw::Text::GetExtent(const char* value, float& width, float& height)
{
    HC_Compute_Text_Extent(".", value, &width, &height);
}
