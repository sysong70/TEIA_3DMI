#include "stdafx.h"
#include "HDraw.h"
#include "HBaseView.h"

USING_3DF_NAMESPACE

namespace HDraw
{
    HBaseView* View = nullptr;
}


void HDraw::Test(HBaseView* view, TDF::Matrix & cMatrix, Point p1, Point p2)
{
    SetView(view);

	Matrix cInverseMatrix;
	cMatrix.ShowInverse(cInverseMatrix);

    Point cInvPo1 = cInverseMatrix.Transform(p1);
    Point cInvPo2 = cInverseMatrix.Transform(p2);

    HC_Open_Segment("test_draw"); {

        HC_Set_Modelling_Matrix(cMatrix.m_fData);

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

                Line::Create(cInvPo1, cInvPo2);
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
                Circle::Create(cInvPo1, radius, false);
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
                Circle::Create(cInvPo2, radius, false);
            }
            HC_Close_Segment();

            // Draw text and outer frame

            TDF::Point center = (cInvPo1 + cInvPo2) / 2.0f;

            Vector cXAixs = cMatrix.XAxis();
            Vector cYAixs = cMatrix.YAxis();
            Vector cZAixs = cMatrix.ZAxis();
            Vector cOrigin = cMatrix.Origin();

            Point2D cDrop1 = p1.DropPoint(cOrigin, cXAixs, cYAixs);
            Point2D cDrop2 = p2.DropPoint(cOrigin, cXAixs, cYAixs);

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

            // Text 회전각도 조절
            if (90.0f < fAngle && fAngle < 270.0f) {
                fAngle = fAngle + 180.0f;
            }

            TDF::Matrix cRotation;
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

                Format value("%.3f mm", Compute::Distance(cInvPo1, cInvPo2));
                float width, height;
                Text::GetExtent(value, width, height);

                //:TODO - calculate point or use segment metrix

                Point cTextCenter = center;
                cTextCenter.y += -height / 2.0f;

                //Text::Create(cTextCenter, value);
                Text::Create(Point(0, -height / 2), value);

                double offset = Compute::PixelToWorld(8);
                Point cFigureCenter = center;
                Point cOffset1(-width / 2, height / 2 + offset);
                Point cOffset2(width / 2, -height / 2 - offset);

                //:TODO - calculate point or use segment metrix
                //Figure::CreateObround(cFigureCenter + cOffset1, cFigureCenter + cOffset2);
                Figure::CreateObround(Point(-width / 2, height / 2 + offset), Point(width / 2, -height / 2 - offset));
            } HC_Close_Segment();
        } HC_Close_Segment();
    } HC_Close_Segment();

    SetView(nullptr);
}



void HDraw::SetView(HBaseView* view)
{
    View = view;
}



void HDraw::Arc::Create(TDF::Point first, TDF::Point second, TDF::Point third)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circular_Arc(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);
}

void HDraw::Arc::GetPoints(TDF::Point center, double radius, double startAngle, double endAngle, HPoints& points)
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

void HDraw::Arc::GetPoints(float x, float y, double radius, double startAngle, double endAngle, HPoints& points)
{
    GetPoints(TDF::Point(x, y), radius, startAngle, endAngle, points);
}



void HDraw::Circle::Create(TDF::Point center, double radius, bool polygon)
{
    center.z = 0;

    if (polygon) {
        HPoints points;

        for (int i = 0; i <= 360; i += 6) {
            double angle = i * 3.141592 / 180;
            TDF::Point p(cos(angle), sin(angle));
            points.push_back(center + (p * radius));
        }

        Polygon::Create(points);
    }
    else {
        HC_KEY key = HC_Insert_Circle_By_Radius(&center, radius, NULL);
        ASSERT(key != HC_ERROR_KEY);
    }
}

void HDraw::Circle::Create(TDF::Point first, TDF::Point second, TDF::Point third, bool polygon)
{
    first.z = 0;
    second.z = 0;
    third.z = 0;

    HC_KEY key = HC_Insert_Circle(&first, &second, &third);
    ASSERT(key != HC_ERROR_KEY);
}



double HDraw::Compute::Distance(TDF::Point p1, TDF::Point p2)
{
    TDF::Point d = p2 - p1;
    return sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
}

double HDraw::Compute::PixelToWorld(double value)
{
    Point points[2];
    points[1].x = value;

    HC_Open_Segment_By_Key(View->GetSceneKey());
    HC_Compute_Coordinates(".", "local pixels", &points[0], "world", &points[0]);
    HC_Compute_Coordinates(".", "local pixels", &points[1], "world", &points[1]);
    HC_Close_Segment();

    return Distance(points[0], points[1]);
}

Vector HDraw::Compute::Normal()
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



void HDraw::Figure::CreateObround(TDF::Point topLeft, TDF::Point bottomRight)
{
    HPoints points;
    TDF::Point center = (bottomRight + topLeft) / 2;
    double dist = fabs(topLeft.y - bottomRight.y) / 2;

    Arc::GetPoints(topLeft.x, center.y, dist, 90, 270, points);
    Arc::GetPoints(bottomRight.x, center.y, dist, 270, 90, points);

    Polygon::Create(points);
}



void HDraw::Line::Create(TDF::Point first, TDF::Point second, bool firstEnd, bool secondEnd)
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



void HDraw::Text::Create(TDF::Point center, const char* value)
{
    HC_KEY key = HC_Insert_Text(center.x, center.y, 0, value);
    ASSERT(key != HC_ERROR_KEY);
}

void HDraw::Text::GetExtent(const char* value, float& width, float& height)
{
    HC_Compute_Text_Extent(".", value, &width, &height);
}
